#include <fcppt/assign/make_container.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/volume/flakes/mover.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/math/dim/output.hpp>


flake::volume::flakes::mover::mover(
	flakelib::cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool,
	flakes::position_view const &_positions,
	flakes::point_size_view const &_point_sizes,
	flakes::snow_density_view const &_snow_density,
	flakes::collision_increment const &_collision_increment,
	flakes::activity_view const &_activity,
	flakes::minimum_size const &_minimum_size,
	flakes::maximum_size const &_maximum_size,
	flakes::gravity_magnitude const &_gravity_magnitude,
	flakelib::marching_cubes::iso_level const &_iso_level)
:
	program_(
		_program_context.command_queue(),
		flake::media_path_from_string(
			FCPPT_TEXT("kernels/flake/flakes/mover.cl")),
		_program_context.compiler_flags()),
	initialize_velocities_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"initialize_velocities"))),
	move_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"move"))),
	update_activity_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"update_activity"))),
	positions_(
		_positions),
	point_sizes_(
		_point_sizes),
	activity_(
		_activity),
	velocities_(
		fcppt::make_unique_ptr<linear_float4_lock>(
			fcppt::ref(
				_buffer_pool),
			sge::opencl::memory_object::dim1(
				_positions.get().size().w()))),
	vertex_count_(
		_positions.get().size().w())
{
	this->initialize_velocities(
		_minimum_size,
		_maximum_size);

	update_activity_kernel_->numerical_argument(
		"iso_level",
		_iso_level.get());

	update_activity_kernel_->buffer_argument(
		"snow_density",
		_snow_density.get().buffer());

	update_activity_kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_snow_density.get().size().w()));

	update_activity_kernel_->buffer_argument(
		"activity",
		_activity.get().buffer());

	move_kernel_->numerical_argument(
		"minimum_size",
		_minimum_size.get());

	move_kernel_->buffer_argument(
		"velocities",
		velocities_->value().buffer());

	move_kernel_->numerical_argument(
		"maximum_size",
		_maximum_size.get());

	move_kernel_->numerical_argument(
		"gravity_magnitude",
		_gravity_magnitude.get());

	move_kernel_->buffer_argument(
		"activity",
		_activity.get().buffer());

	move_kernel_->buffer_argument(
		"sizes",
		_point_sizes.get().buffer());

	move_kernel_->buffer_argument(
		"positions",
		_positions.get().buffer());

	move_kernel_->buffer_argument(
		"snow_density",
		_snow_density.get().buffer());

	move_kernel_->numerical_argument(
		"collision_increment",
		_collision_increment.get());
}

void
flake::volume::flakes::mover::update(
	flakelib::duration const &_delta,
	flakelib::volume::velocity_buffer_view const &_velocity,
	flakelib::volume::boundary_buffer_view const &_boundary)
{
	this->update_activity(
		_boundary);

	move_kernel_->numerical_argument(
		"time_delta",
		_delta.count());

	move_kernel_->buffer_argument(
		"fluid_velocity",
		_velocity.get().buffer());

	move_kernel_->numerical_argument(
		"bounding_volume_width",
		static_cast<cl_int>(
			_boundary.get().size().w()));

	move_kernel_->numerical_argument(
		"bounding_volume_height",
		static_cast<cl_int>(
			_boundary.get().size().h()));

	move_kernel_->numerical_argument(
		"bounding_volume_depth",
		static_cast<cl_int>(
			_boundary.get().size().d()));

	move_kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_velocity.get().size().w()));

	sge::opencl::memory_object::scoped_objects scoped_vb(
		move_kernel_->command_queue(),
		fcppt::assign::make_container<sge::opencl::memory_object::base_ref_sequence>
			(&positions_.get().buffer())
			(&point_sizes_.get().buffer()));

	move_kernel_->enqueue_automatic(
		flakelib::cl::global_dim1(
			sge::opencl::memory_object::dim1(
				vertex_count_)));
}

flake::volume::flakes::mover::~mover()
{
}

void
flake::volume::flakes::mover::initialize_velocities(
	flakes::minimum_size const &_minimum_size,
	flakes::maximum_size const &_maximum_size)
{
	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&point_sizes_.get().buffer());

	sge::opencl::memory_object::scoped_objects scoped_vb(
		move_kernel_->command_queue(),
		mem_objects);

	initialize_velocities_kernel_->numerical_argument(
		"minimum_size",
		_minimum_size.get());

	initialize_velocities_kernel_->numerical_argument(
		"maximum_size",
		_maximum_size.get());

	initialize_velocities_kernel_->buffer_argument(
		"velocities",
		velocities_->value().buffer());

	initialize_velocities_kernel_->buffer_argument(
		"sizes",
		point_sizes_.get().buffer());

	initialize_velocities_kernel_->enqueue_automatic(
		flakelib::cl::global_dim1(
			velocities_->value().size()));
}

void
flake::volume::flakes::mover::update_activity(
	flakelib::volume::boundary_buffer_view const &_boundary)
{
	update_activity_kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	update_activity_kernel_->enqueue_automatic(
		flakelib::cl::global_dim3(
			_boundary.get().size()));
}
