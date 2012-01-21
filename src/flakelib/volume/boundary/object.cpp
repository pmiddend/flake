#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/utility/object.hpp>
#include <flakelib/volume/boundary/object.hpp>
#include <flakelib/volume/boundary/cube/object.hpp>
#include <flakelib/volume/boundary/sphere/object.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/assert/unreachable.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/variant/get.hpp>
#include <fcppt/variant/holds_type.hpp>


flakelib::volume::boundary::object::object(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::utility::object &_utility,
	flakelib::buffer_pool::object &_buffer_pool,
	flakelib::build_options const &_build_options,
	volume::grid_size const &_grid_size,
	boundary::obstacle_sequence const &_obstacles,
	boundary::pave_ground const &_pave_ground)
:
	command_queue_(
		_command_queue),
	buffer_lock_(
		_buffer_pool,
		_grid_size.get()),
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

	if(_pave_ground.get())
	{
		this->add(
			boundary::cube::object(
				boundary::cube::size(
					sge::opencl::memory_object::dim3(
						_grid_size.get()[0],
						1,
						_grid_size.get()[2])),
				boundary::cube::position(
					sge::opencl::memory_object::dim3(
						0,
						0,
						0))));
	}

	for(
		boundary::obstacle_sequence::const_iterator it =
			_obstacles.begin();
		it != _obstacles.end();
		++it)
	{
		// This could be a visitor, but I'm too lazy
		if(fcppt::variant::holds_type<boundary::sphere::object>(*it))
		{
			this->add(
				fcppt::variant::get<boundary::sphere::object>(
					*it));
			continue;
		}
		else if(fcppt::variant::holds_type<boundary::cube::object>(*it))
		{
			this->add(
				fcppt::variant::get<boundary::cube::object>(
					*it));
			continue;

		}

		FCPPT_ASSERT_UNREACHABLE;
	}
}

flakelib::volume::boundary::view::value_type const
flakelib::volume::boundary::object::get()
{
	return buffer_lock_.value();
}

flakelib::volume::boundary::object::~object()
{
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
			_cube.size().get()[0]));

	add_cube_kernel_.argument(
		sge::opencl::kernel::argument_index(
			5),
		static_cast<cl_int>(
			_cube.size().get()[1]));

	add_cube_kernel_.argument(
		sge::opencl::kernel::argument_index(
			6),
		static_cast<cl_int>(
			_cube.size().get()[2]));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		add_cube_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(buffer_lock_.value().size()[0])
			(buffer_lock_.value().size()[1])
			(buffer_lock_.value().size()[2]).container());
}
