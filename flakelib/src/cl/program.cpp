#include <flakelib/exception.hpp>
#include <flakelib/log.hpp>
#include <flakelib/map_key_sequence.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/from_std_string.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/optional_impl.hpp>
#include <fcppt/algorithm/contains.hpp>
#include <fcppt/algorithm/shortest_levenshtein.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/io/stream_to_string.hpp>
#include <fcppt/log/headers.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/next_prior.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/range/adaptor/map.hpp>
#include <fstream>
#include <set>
#include <utility>
#include <fcppt/config/external_end.hpp>


flakelib::cl::program::program(
	sge::opencl::command_queue::object &_command_queue,
	boost::filesystem::path const &_file_path,
	flakelib::cl::compiler_flags const &_compiler_flags)
:
	command_queue_(
		_command_queue),
	program_(
		_command_queue.context(),
		sge::opencl::program::file_to_source_string_sequence(
			_file_path),
		sge::opencl::program::optional_build_parameters()),
	kernel_name_to_parameters_()
{
	try
	{
		program_.build(
			sge::opencl::program::build_parameters()
				.options(
					_compiler_flags.get()));
	}
	catch(fcppt::exception const &e)
	{
		throw
			flakelib::exception(
				FCPPT_TEXT("Error compiling program:\n\n")+
				fcppt::filesystem::path_to_string(
					_file_path)+
				FCPPT_TEXT(":\n\n")+
				e.string());
	}

	FCPPT_LOG_DEBUG(
		flakelib::log(),
		fcppt::log::_
			<< FCPPT_TEXT("Parsing OpenCL program: ")
			<< fcppt::filesystem::path_to_string(_file_path));

	this->fill_kernel_name_to_parameters(
		_file_path);

	if(kernel_name_to_parameters_.empty())
	{
		throw
			flakelib::exception(
				FCPPT_TEXT("The program\n\n")+
				fcppt::filesystem::path_to_string(
					_file_path)+
				FCPPT_TEXT("\n\nDoesn't have any kernels defined."));
	}

	FCPPT_LOG_DEBUG(
		flakelib::log(),
		fcppt::log::_
			<< FCPPT_TEXT("Program successfully parsed!"));
}

flakelib::cl::kernel_unique_ptr
flakelib::cl::program::create_kernel(
	sge::opencl::kernel::name const &_kernel_name)
{
	kernel_name_to_parameters::const_iterator const kernel_name_it =
		kernel_name_to_parameters_.find(
			_kernel_name.get());

	if(kernel_name_it == kernel_name_to_parameters_.end())
	{
		throw
			flakelib::exception(
				FCPPT_TEXT("Kernel called \"")+
				fcppt::from_std_string(
					_kernel_name.get())+
				FCPPT_TEXT("\" not found, did you mean \"")+
				fcppt::from_std_string(
					fcppt::algorithm::shortest_levenshtein(
						flakelib::map_key_sequence(
							kernel_name_to_parameters_),
						_kernel_name.get()))+
				FCPPT_TEXT("\"?"));
	}

	return
		fcppt::make_unique_ptr<cl::kernel>(
			program_,
			command_queue_,
			_kernel_name,
			kernel_name_it->second);
}

sge::opencl::program::object &
flakelib::cl::program::impl()
{
	return
		program_;
}

flakelib::cl::program::~program()
{
}

void
flakelib::cl::program::fill_kernel_name_to_parameters(
	boost::filesystem::path const &_file_path)
{
	boost::filesystem::ifstream s(
		_file_path);

	if(!s.is_open())
		throw
			flakelib::exception(
				FCPPT_TEXT("File \n\n")+
				fcppt::filesystem::path_to_string(
					_file_path)+
				FCPPT_TEXT("\n\nCould not be opened!"));

	std::string const content =
		fcppt::io::stream_to_string(
			s);

	std::string const
		kernel_name_search_term =
			"FLAKELIB_KERNEL_NAME",
		kernel_argument_search_term =
			"FLAKELIB_KERNEL_ARGUMENT";

	typedef
	std::set<std::string::size_type>
	position_set;

	position_set kernel_name_positions,kernel_argument_positions;

	std::string::size_type search_from_this_position = 0;
	std::string::size_type next_kernel_position;
	while(
		(next_kernel_position =
			content.find(
				kernel_name_search_term,
				search_from_this_position)) != std::string::npos)
	{
		kernel_name_positions.insert(
			next_kernel_position);

		search_from_this_position =
			next_kernel_position + kernel_name_search_term.length();
	}

	FCPPT_LOG_DEBUG(
		flakelib::log(),
		fcppt::log::_
			<< FCPPT_TEXT("Extracted ")
			<< kernel_name_positions.size()
			<< FCPPT_TEXT(" kernel names"));

	search_from_this_position = 0;
	std::string::size_type next_kernel_argument_position;
	while(
		(next_kernel_argument_position =
			content.find(
				kernel_argument_search_term,
				search_from_this_position)) != std::string::npos)
	{
		kernel_argument_positions.insert(
			next_kernel_argument_position);

		search_from_this_position =
			next_kernel_argument_position + kernel_argument_search_term.length();
	}

	FCPPT_LOG_DEBUG(
		flakelib::log(),
		fcppt::log::_
			<< FCPPT_TEXT("Extracted ")
			<< kernel_argument_positions.size()
			<< FCPPT_TEXT(" kernel argument names"));


	for(
		position_set::const_iterator
			current_kernel_position =
				kernel_name_positions.begin();
		current_kernel_position != kernel_name_positions.end();
		++current_kernel_position)
	{
		std::string::size_type const
			after_opening_brace =
				static_cast<std::string::size_type>(
					*current_kernel_position +
					kernel_name_search_term.length() +
					1),
			on_closing_brace =
				content.find(
					')',
					after_opening_brace);

		if(after_opening_brace > content.length() || on_closing_brace == std::string::npos)
		{
			throw
				flakelib::exception(
					FCPPT_TEXT("Couldn't find a closing brace for opening brace"));
		}

		std::string const kernel_name(
			content.substr(
				after_opening_brace,
				static_cast<std::string::size_type>(
					on_closing_brace - after_opening_brace)));

		FCPPT_LOG_DEBUG(
			flakelib::log(),
			fcppt::log::_
				<< FCPPT_TEXT("Extracted kernel name: ")
				<< fcppt::from_std_string(kernel_name));

		if(kernel_name_to_parameters_.find(kernel_name) != kernel_name_to_parameters_.end())
		{
			throw
				flakelib::exception(
					FCPPT_TEXT("Kernel name \"")+
					fcppt::from_std_string(
						kernel_name)+
					FCPPT_TEXT("\" appears twice in program"));
		}

		// Insert and retrieve the inserted value.
		cl::kernel_parameters &parameters =
			kernel_name_to_parameters_.insert(
				std::make_pair(
					kernel_name,
					cl::kernel_parameters())).first->second;

		bool const next_position_available =
			current_kernel_position !=
			boost::prior(
				kernel_name_positions.end());

		for(
			position_set::const_iterator
				current_argument_position =
					// First position greater than or equal to kernel name position
					kernel_argument_positions.lower_bound(
						*current_kernel_position),
				one_past_last_argument =
					// First position greater than or equal to _next_ kernel name position
					kernel_argument_positions.lower_bound(
						next_position_available
						?
							*boost::next(
								current_kernel_position)
						:
							content.length());
			current_argument_position != one_past_last_argument;
			++current_argument_position)
		{

			std::string::size_type const
				argument_after_opening_brace =
					static_cast<std::string::size_type>(
						*current_argument_position +
						kernel_argument_search_term.length() +
						1),
				argument_on_closing_brace =
					content.find(
						')',
						argument_after_opening_brace);

			if(argument_after_opening_brace > content.length() || argument_on_closing_brace == std::string::npos)
			{
				throw
					flakelib::exception(
						FCPPT_TEXT("Couldn't find a closing brace for opening brace"));
			}

			std::string const kernel_argument_name =
				content.substr(
					argument_after_opening_brace,
					static_cast<std::string::size_type>(
						argument_on_closing_brace - argument_after_opening_brace));

			FCPPT_LOG_DEBUG(
				flakelib::log(),
				fcppt::log::_
					<< FCPPT_TEXT("Extracted kernel argument: ")
					<< fcppt::from_std_string(kernel_argument_name));

			if(fcppt::algorithm::contains(parameters,kernel_argument_name))
			{
				throw
					flakelib::exception(
						FCPPT_TEXT("Kernel argument name \"")+
						fcppt::from_std_string(
							kernel_argument_name)+
						FCPPT_TEXT("\" appears twice in program"));
			}

			parameters.push_back(
				kernel_argument_name);
		}
	}
}
