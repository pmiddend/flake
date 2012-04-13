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
	flakes::position_view const &_positions)
:
	program_(
		_program_context.command_queue(),
		flake::media_path_from_string(
			FCPPT_TEXT("kernels/flake/flakes/mover.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"apply"))),
	positions_(
		_positions),
	vertex_count_(
		_positions.get().size().w())
{
	kernel_->buffer_argument(
		"positions",
		_positions.get().buffer());
}

void
flake::volume::flakes::mover::update(
	flakelib::duration const &_delta,
	flakelib::volume::velocity_buffer_view const &_velocity,
	flakelib::volume::boundary_buffer_view const &_boundary)
{
	kernel_->numerical_argument(
		"time_delta",
		_delta.count());

	kernel_->buffer_argument(
		"velocity",
		_velocity.get().buffer());

	kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	kernel_->numerical_argument(
		"bounding_volume_width",
		static_cast<cl_int>(
			_boundary.get().size().w()));

	kernel_->numerical_argument(
		"bounding_volume_height",
		static_cast<cl_int>(
			_boundary.get().size().h()));

	kernel_->numerical_argument(
		"bounding_volume_depth",
		static_cast<cl_int>(
			_boundary.get().size().d()));

	kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_velocity.get().size().w()));

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&positions_.get().buffer());

	sge::opencl::memory_object::scoped_objects scoped_vb(
		kernel_->command_queue(),
		mem_objects);

	kernel_->enqueue_automatic(
		sge::opencl::memory_object::dim1(
			vertex_count_));
}

flake::volume::flakes::mover::~mover()
{
}
