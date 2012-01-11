#include <flakelib/buffer/linear_view.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/utility/object.hpp>
#include <flakelib/volume/boundary/object.hpp>
#include <flakelib/volume/boundary/sphere/object.hpp>
#include <flakelib/volume/boundary/cube/object.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/assign/make_array.hpp>


flakelib::volume::boundary::object::object(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::utility::object &_utility,
	flakelib::buffer_pool::object &_buffer_pool,
	flakelib::build_options const &_build_options,
	sge::opencl::memory_object::dim3 const &_size)
:
	command_queue_(
		_command_queue),
	buffer_lock_(
		_buffer_pool,
		_size),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/volume/boundary.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	add_sphere_kernel_(
		program_,
		sge::opencl::kernel::name(
			"add_sphere")),
	add_cube_kernel_(
		program_,
		sge::opencl::kernel::name(
			"add_cube"))
{
	_utility.null_buffer(
		buffer::linear_view<cl_float>(
			buffer_lock_.value().buffer()));
}

flakelib::volume::boundary::view::value_type const
flakelib::volume::boundary::object::get()
{
	return buffer_lock_.value();
}

void
flakelib::volume::boundary::object::add(
	boundary::sphere::object const &_sphere)
{
	add_sphere_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		buffer_lock_.value().buffer());

	add_sphere_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		static_cast<cl_int>(
			_sphere.position().get()[0]));

	add_sphere_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_int>(
			_sphere.position().get()[1]));

	add_sphere_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_int>(
			_sphere.position().get()[2]));

	add_sphere_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		static_cast<cl_int>(
			_sphere.radius().get()));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		add_sphere_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(buffer_lock_.value().size()[0])
			(buffer_lock_.value().size()[1])
			(buffer_lock_.value().size()[2]).container());
}

void
flakelib::volume::boundary::object::add(
	boundary::cube::object const &_cube)
{
	add_cube_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		buffer_lock_.value().buffer());

	add_cube_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		static_cast<cl_int>(
			_cube.position().get()[0]));

	add_cube_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_int>(
			_cube.position().get()[1]));

	add_cube_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_int>(
			_cube.position().get()[2]));

	add_cube_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		static_cast<cl_int>(
			_cube.width().get()));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		add_cube_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(buffer_lock_.value().size()[0])
			(buffer_lock_.value().size()[1])
			(buffer_lock_.value().size()[2]).container());
}

flakelib::volume::boundary::object::~object()
{
}
