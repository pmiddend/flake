#include <flakelib/exception.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/float_buffer_lock.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/opencl/command_queue/map_flags.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/event/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <fcppt/const.hpp>
#include <fcppt/from_std_string.hpp>
#include <fcppt/make_int_range_count.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/container/raw_vector.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/math/dim/contents.hpp>
#include <fcppt/math/dim/is_quadratic.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/optional/from.hpp>
#include <fcppt/optional/object.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/fstream.hpp>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <fcppt/config/external_end.hpp>


flakelib::volume::conversion::object::object(
	cl::program_context const &_program_context)
:
	program_(
		_program_context.log_context(),
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/volume/conversion.cl")),
		_program_context.compiler_flags()),
	to_arrow_vb_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"to_arrow_vb"))),
	float_view_to_flat_volume_texture_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"float_view_to_flat_volume_texture")))
{
}

void
flakelib::volume::conversion::object::to_arrow_vb(
	volume::float4_view const &_input,
	sge::opencl::memory_object::buffer &_output,
	conversion::arrow_scale const &_arrow_scale,
	conversion::origin const &_origin,
	conversion::grid_scale const &_grid_scale)
{
        sge::opencl::memory_object::base_ref_sequence mem_objects;

	mem_objects.push_back(
		&_output);

        sge::opencl::memory_object::scoped_objects scoped_vb(
		to_arrow_vb_kernel_->command_queue(),
		mem_objects);

	to_arrow_vb_kernel_->buffer_argument(
		"input",
		_input.buffer());

	to_arrow_vb_kernel_->buffer_argument(
		"output",
		_output);

	to_arrow_vb_kernel_->numerical_argument(
		"input_line_pitch",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_input.size().w())));

	to_arrow_vb_kernel_->numerical_argument(
		"arrow_scale",
		sge::opencl::kernel::numeric_type(
			_arrow_scale.get()));

	to_arrow_vb_kernel_->numerical_argument(
		"grid_scale",
		sge::opencl::kernel::numeric_type(
			_grid_scale.get()));

	to_arrow_vb_kernel_->numerical_argument(
		"origin_x",
		sge::opencl::kernel::numeric_type(
			_origin.get().x()));

	to_arrow_vb_kernel_->numerical_argument(
		"origin_y",
		sge::opencl::kernel::numeric_type(
			_origin.get().y()));

	to_arrow_vb_kernel_->numerical_argument(
		"origin_z",
		sge::opencl::kernel::numeric_type(
			_origin.get().z()));

	to_arrow_vb_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim3(
			_input.size()));
}

void
flakelib::volume::conversion::object::float_view_to_flat_volume_texture(
	volume::float_view const &_input,
	sge::opencl::memory_object::image::planar &_output,
	conversion::scaling_factor const &_scaling_factor,
	conversion::constant_addition const &_constant_addition)
{
	FCPPT_ASSERT_PRE(
		fcppt::math::dim::is_quadratic(
			_input.size()));

	sge::opencl::memory_object::base_ref_sequence mem_objects;

	mem_objects.push_back(
		&_output);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		float_view_to_flat_volume_texture_kernel_->command_queue(),
		mem_objects);

	float_view_to_flat_volume_texture_kernel_->buffer_argument(
		"input",
		_input.buffer());

	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"input_line_pitch",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_input.size().w())));

	float_view_to_flat_volume_texture_kernel_->planar_image_argument(
		"output",
		_output);

	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"scaling_factor",
		sge::opencl::kernel::numeric_type(
			_scaling_factor.get()));

	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"constant_addition",
		sge::opencl::kernel::numeric_type(
			_constant_addition.get()));

	// This is the place where the assertion above comes in.
	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"slice_edge_size",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_input.size().w())));

	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"slices_per_row",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_output.size().w() / _input.size().w())));

	float_view_to_flat_volume_texture_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim3(
			_input.size()));
}

flakelib::volume::unique_float_buffer_lock
flakelib::volume::conversion::object::raw_voxel_file_to_buffer(
	flakelib::buffer_pool::object &_buffer_pool,
	boost::filesystem::path const &_path,
	flakelib::volume::conversion::raw_voxel_file_dimension const &_raw_voxel_file_dimension)
{
	flakelib::volume::unique_float_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
			_buffer_pool,
			sge::opencl::dim3(
				_raw_voxel_file_dimension.get(),
				_raw_voxel_file_dimension.get(),
				_raw_voxel_file_dimension.get())));

	boost::filesystem::ifstream input_file(
		_path,
		std::ios::binary);

	if(!input_file.is_open())
		throw
			flakelib::exception(
				FCPPT_TEXT("Couldn't load \"")+
				fcppt::filesystem::path_to_string(
					_path)+
				FCPPT_TEXT("\""));

	sge::opencl::command_queue::scoped_buffer_mapping scoped_mapping(
		to_arrow_vb_kernel_->command_queue(),
		result->value().buffer(),
		sge::opencl::command_queue::map_flags::write,
		sge::opencl::memory_object::byte_offset(
			0u),
		result->value().buffer().byte_size(),
		sge::opencl::event::sequence());

	cl_float *fptr =
		static_cast<cl_float *>(
			scoped_mapping.ptr());

	for(
		std::size_t const i
		:
		fcppt::make_int_range_count(
			fcppt::math::dim::contents(
				result->value().size()
			)
		)
	)
	{
		unsigned char new_char;

		input_file.read(
			reinterpret_cast<char *>(
				&new_char),
			1u);

		if(i > 0 && i % 64 == 0)
			std::cout << "\n";

		if(i > 0 && i % (64 * 64) == 0)
			std::cout << "\n";

		std::cout << (static_cast<unsigned int>(new_char) ? 1 : 0);

		*fptr++ =
			new_char
			?
				1.0f
			:
				0.0f;
	}

	std::cout.flush();

	return
		result;
}

flakelib::volume::conversion::object::~object()
{
}

flakelib::volume::unique_float_buffer_lock
flakelib::volume::conversion::object::binvox_file_to_buffer(
	flakelib::buffer_pool::object &_buffer_pool,
	boost::filesystem::path const &_path,
	flakelib::volume::conversion::optional_height const &_optional_height)
{
	std::cout << "binvox_file_to_buffer\n";
	boost::filesystem::ifstream input_file(
		_path,
		std::ios::binary);

	if(!input_file.is_open())
		throw
			flakelib::exception(
				FCPPT_TEXT("Couldn't load \"")+
				fcppt::filesystem::path_to_string(
					_path)+
				FCPPT_TEXT("\""));

	std::string line;

	if(!std::getline(input_file,line))
		throw
			flakelib::exception(
				FCPPT_TEXT("Couldn't load \"")+
				fcppt::filesystem::path_to_string(
					_path)+
				FCPPT_TEXT("\": First line broken"));

	if(line != "#binvox 1")
		throw
			flakelib::exception(
				FCPPT_TEXT("Couldn't load \"")+
				fcppt::filesystem::path_to_string(
					_path)+
				FCPPT_TEXT("\": First line is\n\n")+
				fcppt::from_std_string(
					line)+
				FCPPT_TEXT("\n\ninstead of \"#binvox 1\"!"));

	std::cerr << "binvox: First line is correct\n";

	fcppt::optional::object<std::size_t> optional_dimension;
	while(!optional_dimension.has_value())
	{
		if(!std::getline(input_file,line))
			throw
				flakelib::exception(
					FCPPT_TEXT("Couldn't load \"")+
					fcppt::filesystem::path_to_string(
						_path)+
					FCPPT_TEXT("\": Some line broken or unexpected end of file"));

		if(line == "data")
			throw
				flakelib::exception(
					FCPPT_TEXT("Couldn't load \"")+
					fcppt::filesystem::path_to_string(
						_path)+
					FCPPT_TEXT("\": No dim found"));

		std::string::size_type const space_position =
			line.find(' ');

		if(space_position == std::string::npos)
			throw
				flakelib::exception(
					FCPPT_TEXT("Couldn't load \"")+
					fcppt::filesystem::path_to_string(
						_path)+
					FCPPT_TEXT("\": Line without prefix found"));

		std::string const prefix(
			line.substr(
				0u,
				space_position));

		if(prefix != "dim")
			continue;

		std::istringstream ss(
			line.substr(
				space_position+1u));

		std::size_t dims[3];
		if(!(ss >> dims[0] >> dims[1] >> dims[2]))
			throw
				flakelib::exception(
					FCPPT_TEXT("Couldn't load \"")+
					fcppt::filesystem::path_to_string(
						_path)+
					FCPPT_TEXT("\": Invalid string after \"dim\" found"));

		if(dims[0] != dims[1] || dims[1] != dims[2])
			throw
				flakelib::exception(
					FCPPT_TEXT("Couldn't load \"")+
					fcppt::filesystem::path_to_string(
						_path)+
					FCPPT_TEXT("\": Dimension is not equal on all axes"));

		optional_dimension =
			fcppt::optional::object<std::size_t>(
				dims[0]);
	}

	std::size_t const dimension(
		optional_dimension.get_unsafe());

	std::cerr << "binvox: Got the dimension: " << dimension << "\n";

	bool data_found =
		false;
	while(
		std::getline(
			input_file,
			line))
	{
		if(line == "data")
		{
			data_found = true;
			break;
		}
	}

	std::cerr << "binvox: Found the data segment\n";

	if(!data_found)
		throw
			flakelib::exception(
				FCPPT_TEXT("Couldn't load \"")+
				fcppt::filesystem::path_to_string(
					_path)+
				FCPPT_TEXT("\": Data section not found"));

	// Read all bytes until the end
	std::streampos const current_position =
		input_file.tellg();

	input_file.seekg(
		0,
		std::ios_base::end);

	std::streampos const end_position =
		input_file.tellg();

	input_file.seekg(
		current_position,
		std::ios_base::beg);

	typedef
	fcppt::container::raw_vector<unsigned char>
	byte_sequence;

	byte_sequence data_section(
		static_cast<byte_sequence::size_type>(
			end_position - current_position));

	input_file.read(
		reinterpret_cast<char *>(
			data_section.data()),
		static_cast<std::streamsize>(
			data_section.size()));

	std::cerr << "binvox: Read the data segment: " << data_section.size() << " bytes\n";

	if(data_section.size() % 2u != 0u)
		throw
			flakelib::exception(
				FCPPT_TEXT("Couldn't load \"")+
				fcppt::filesystem::path_to_string(
					_path)+
				FCPPT_TEXT("\": Data section not multiple of two long"));

	sge::opencl::dim3 const real_grid_size(
		dimension,
		fcppt::optional::from(_optional_height, fcppt::const_(dimension)),
		dimension);

	flakelib::volume::unique_float_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
			_buffer_pool,
			real_grid_size));

	sge::opencl::command_queue::scoped_buffer_mapping scoped_mapping(
		to_arrow_vb_kernel_->command_queue(),
		result->value().buffer(),
		sge::opencl::command_queue::map_flags::write,
		sge::opencl::memory_object::byte_offset(
			0u),
		result->value().buffer().byte_size(),
		sge::opencl::event::sequence());

	cl_float *fptr =
		static_cast<cl_float *>(
			scoped_mapping.ptr());

	std::size_t current_index =
		0u;

	for(
		byte_sequence::size_type
			input_index =
				0u;
		input_index != data_section.size();
		input_index += 2u)
	{
		cl_float const value =
			data_section[input_index] ? 1.0f : 0.0f;

		unsigned const
			count =
				static_cast<unsigned char>(
					data_section[input_index+1u]);

		for(unsigned i = 0; i < count; ++i)
		{
			std::size_t const
				y =
					current_index % dimension,
				z =
					(current_index / dimension) % dimension,
				x =
					current_index / (dimension * dimension);

			current_index++;

			if(y >= real_grid_size.h())
				continue;

			// Remap z
			fptr[(real_grid_size.w() * real_grid_size.h()) * (real_grid_size.d() - 1u - z) + real_grid_size.w() * y + x] =
				value;

			/* DEBUG OUTPUT
			fptr[current_index] = value;

			std::cout << static_cast<int>(data_section[input_index]);

			if(current_index % dimension == 0u)
				std::cout << "\n";

			if(current_index > dimension && current_index % (dimension * dimension) == 0u)
			{
				std::cout << "\n";
				static unsigned slice_index = 1;
				std::cout << "Slice " << ++slice_index << "\n";
			}
			*/
		}
	}

	std::cout << "current_index: " << current_index << "\n";

	return
		result;
}
