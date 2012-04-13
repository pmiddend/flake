#include <fcppt/math/dim/output.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flake/volume/flakes/mover.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flake/media_path_from_string.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>

flake::volume::flakes::mover::mover(
	flakelib::cl::program_context const &_program_context,
	flakes::position_view const &_positions,
	flakes::snow_density_view const &_snow_density,
	flakes::collision_increment const &_collision_increment,
	flakes::activity_view const &_activity,
	flakelib::marching_cubes::iso_level const &_iso_level)
:
	program_(
		_program_context.command_queue(),
		flake::media_path_from_string(
			FCPPT_TEXT("kernels/flake/flakes/mover.cl")),
		_program_context.compiler_flags()),
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
	activity_(
		_activity),
	vertex_count_(
		_positions.get().size().w())
{
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

	move_kernel_->buffer_argument(
		"activity",
		_activity.get().buffer());

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
	update_activity_kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	update_activity_kernel_->enqueue_automatic(
		_boundary.get().size());

	move_kernel_->numerical_argument(
		"time_delta",
		_delta.count());

	move_kernel_->buffer_argument(
		"velocity",
		_velocity.get().buffer());

	/*
	move_kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());
		*/

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

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&positions_.get().buffer());

	sge::opencl::memory_object::scoped_objects scoped_vb(
		move_kernel_->command_queue(),
		mem_objects);

	move_kernel_->enqueue_automatic(
		sge::opencl::memory_object::dim1(
			vertex_count_));
}

flake::volume::flakes::mover::~mover()
{
}
