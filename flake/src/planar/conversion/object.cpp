#include <flake/exception.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/planar/conversion/object.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/map_flags.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/event/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/variant/holds_type.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/fstream.hpp>
#include <fcppt/config/external_end.hpp>


flake::planar::conversion::object::object(
	flakelib::cl::program_context const &_program_context)
:
	command_queue_(
		_program_context.command_queue()),
	program_(
		_program_context.log_context(),
		_program_context.command_queue(),
		flake::media_path_from_string(
			FCPPT_TEXT("kernels/flake/planar/vertex_buffer_conversions.cl")),
		_program_context.compiler_flags()),
	to_arrow_vb_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"to_arrow_vb"))),
	scalar_to_texture_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"scalar_to_texture")))
{
}

void
flake::planar::conversion::object::to_arrow_vb(
	flakelib::planar::float2_view const &_planar_buffer,
	sge::opencl::memory_object::buffer &_vb,
	conversion::grid_scale const &_grid_scale,
	conversion::arrow_scale const &_arrow_scale)
{
	to_arrow_vb_kernel_->buffer_argument(
		"vb",
		_vb);

	to_arrow_vb_kernel_->buffer_argument(
		"buffer",
		_planar_buffer.buffer());

	to_arrow_vb_kernel_->numerical_argument(
		"buffer_pitch",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_planar_buffer.size().w())));

	to_arrow_vb_kernel_->numerical_argument(
		"grid_scale",
		sge::opencl::kernel::numeric_type(
			_grid_scale.get()));

	to_arrow_vb_kernel_->numerical_argument(
		"arrow_scale",
		sge::opencl::kernel::numeric_type(
			_arrow_scale.get()));

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_vb);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	to_arrow_vb_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim2(
			_planar_buffer.size()));
}

void
flake::planar::conversion::object::arrows_to_matlab_files(
	flakelib::planar::float2_view const &_view,
	flake::planar::conversion::x_coordinate_file const &_x_coordinate,
	flake::planar::conversion::y_coordinate_file const &_y_coordinate)
{
	boost::filesystem::ofstream
		x_file_stream(
			_x_coordinate.get()),
		y_file_stream(
			_y_coordinate.get());

	if(!x_file_stream.is_open())
		throw
			flake::exception(
				FCPPT_TEXT("Couldn't open file \"")+
				fcppt::filesystem::path_to_string(
					_x_coordinate.get())+
				FCPPT_TEXT("\""));

	if(!y_file_stream.is_open())
		throw
			flake::exception(
				FCPPT_TEXT("Couldn't open file \"")+
				fcppt::filesystem::path_to_string(
					_y_coordinate.get())+
				FCPPT_TEXT("\""));

	sge::opencl::command_queue::scoped_buffer_mapping buffer_mapping(
		command_queue_,
		_view.buffer(),
		sge::opencl::command_queue::map_flags::read,
		sge::opencl::memory_object::byte_offset(
			0u),
		_view.buffer().byte_size(),
		sge::opencl::event::sequence());

	cl_float const *p =
		static_cast<cl_float const *>(
			buffer_mapping.ptr());

	for(sge::opencl::size_type y = 0; y < _view.size().h(); ++y)
	{
		for(sge::opencl::size_type x = 0; x < _view.size().w(); ++x)
		{
			x_file_stream << (*p++);
			y_file_stream << (*p++);
			if(x < (_view.size().w()-1u))
			{
				x_file_stream << ",";
				y_file_stream << ",";
			}
		}

		if(y < (_view.size().h()-1u))
		{
			x_file_stream << "\n";
			y_file_stream << "\n";
		}
	}
}

void
flake::planar::conversion::object::scalar_to_matlab_file(
	flakelib::planar::float_view const &_view,
	boost::filesystem::path const &_file)
{
	boost::filesystem::ofstream file_stream(
		_file);

	if(!file_stream.is_open())
		throw
			flake::exception(
				FCPPT_TEXT("Couldn't open file \"")+
				fcppt::filesystem::path_to_string(
					_file)+
				FCPPT_TEXT("\""));

	sge::opencl::command_queue::scoped_buffer_mapping buffer_mapping(
		command_queue_,
		_view.buffer(),
		sge::opencl::command_queue::map_flags::read,
		sge::opencl::memory_object::byte_offset(
			0u),
		_view.buffer().byte_size(),
		sge::opencl::event::sequence());

	cl_float const *p =
		static_cast<cl_float const *>(
			buffer_mapping.ptr());

	for(sge::opencl::size_type y = 0; y < _view.size().h(); ++y)
	{
		for(sge::opencl::size_type x = 0; x < _view.size().w(); ++x)
		{
			file_stream << (*p++);
			if(x < (_view.size().w()-1u))
				file_stream << ",";
		}
		if(y < (_view.size().h()-1u))
			file_stream << "\n";
	}
}

void
flake::planar::conversion::object::scalar_to_texture(
	flakelib::planar::float_view const &_planar_buffer,
	sge::opencl::memory_object::image::planar &_texture,
	conversion::scaling_factor const &_scaling,
	conversion::constant_addition const &_constant_addition)
{
	FCPPT_ASSERT_PRE(
		_planar_buffer.size() == _texture.size());

	scalar_to_texture_kernel_->buffer_argument(
		"input",
		_planar_buffer.buffer());

	scalar_to_texture_kernel_->buffer_argument(
		"output",
		_texture);

	scalar_to_texture_kernel_->numerical_argument(
		"scaling",
		sge::opencl::kernel::numeric_type(
			_scaling.get()));

	scalar_to_texture_kernel_->numerical_argument(
		"addition",
		sge::opencl::kernel::numeric_type(
			_constant_addition.get()));

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_texture);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	scalar_to_texture_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim2(
			_planar_buffer.size()));
}

flake::planar::conversion::object::~object()
{
}
