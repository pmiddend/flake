#include <flakelib/exception.hpp>
#include <flakelib/cl/kernel.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <fcppt/from_std_string.hpp>
#include <fcppt/text.hpp>
#include <fcppt/algorithm/shortest_levenshtein.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/algorithm/string/join.hpp>
#include <algorithm>
#include <iterator>
#include <fcppt/config/external_end.hpp>

flakelib::cl::kernel::kernel(
	sge::opencl::program::object &_program,
	sge::opencl::command_queue::object &_command_queue,
	sge::opencl::kernel::name const &_name,
	cl::kernel_parameters const &_kernel_parameters)
:
	command_queue_(
		_command_queue),
	name_(
		_name),
	kernel_(
		_program,
		_name),
	kernel_parameters_(
		_kernel_parameters),
	unassigned_parameters_()
{
	if(_kernel_parameters.empty())
	{
		throw
			flakelib::exception(
				FCPPT_TEXT("The kernel\n\n")+
				fcppt::from_std_string(
					_name.get())+
				FCPPT_TEXT("\n\nDoesn't have any parameters defined."));
	}

	std::copy(
		kernel_parameters_.begin(),
		kernel_parameters_.end(),
		std::inserter(
			unassigned_parameters_,
			unassigned_parameters_.end()));
}

void
flakelib::cl::kernel::numerical_argument(
	std::string const &_name,
	sge::opencl::kernel::numeric_type const &_value)
{
	unassigned_parameters_.erase(
		_name);

	kernel_.argument(
		this->index_for_name(
			_name),
		_value);
}

void
flakelib::cl::kernel::buffer_argument(
	std::string const &_name,
	sge::opencl::memory_object::base &_memory_object)
{
	unassigned_parameters_.erase(
		_name);

	kernel_.argument(
		this->index_for_name(
			_name),
		_memory_object);
}

void
flakelib::cl::kernel::enqueue_automatic(
	sge::opencl::memory_object::dim1 const &_d)
{
	if(!unassigned_parameters_.empty())
		throw
			flakelib::exception(
				FCPPT_TEXT("The following arguments to the kernel\n\n")+
				fcppt::from_std_string(
					name_.get())+
				FCPPT_TEXT("\n\nhave not been assigned:")+
				fcppt::from_std_string(
					boost::algorithm::join(
						unassigned_parameters_,
						std::string(
							"\n"))));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_d[0]).container());
}

void
flakelib::cl::kernel::enqueue_automatic(
	sge::opencl::memory_object::dim2 const &_d)
{
	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_d[0])
			(_d[1]).container());
}

void
flakelib::cl::kernel::enqueue_automatic(
	sge::opencl::memory_object::dim3 const &_d)
{
	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_d[0])
			(_d[1])
			(_d[2]).container());
}

flakelib::cl::kernel::~kernel()
{
}

sge::opencl::kernel::argument_index const
flakelib::cl::kernel::index_for_name(
	std::string const &_name)
{
	cl::kernel_parameters::const_iterator const it =
		std::find(
			kernel_parameters_.begin(),
			kernel_parameters_.end(),
			_name);

	if(it == kernel_parameters_.end())
		throw
			flakelib::exception(
				FCPPT_TEXT("Kernel parameter \"")+
				fcppt::from_std_string(
					_name)+
				FCPPT_TEXT("\" not found. Did you mean \"")+
				fcppt::from_std_string(
					fcppt::algorithm::shortest_levenshtein(
						kernel_parameters_,
						_name))+
				FCPPT_TEXT("\"?"));

	return
		sge::opencl::kernel::argument_index(
			static_cast<sge::opencl::kernel::argument_index::value_type>(
				std::distance(
					kernel_parameters_.begin(),
					it)));
}
