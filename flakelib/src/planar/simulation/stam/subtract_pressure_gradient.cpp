#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/planar/simulation/stam/subtract_pressure_gradient.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::planar::simulation::stam::subtract_pressure_gradient::subtract_pressure_gradient(
	cl::program_context const &_program_context)
:
	program_(
		_program_context.log_context(),
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/planar/subtract_pressure_gradient.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"subtract_pressure_gradient_apply")))
{
}

void
flakelib::planar::simulation::stam::subtract_pressure_gradient::update(
	planar::velocity_buffer_view const &_velocity,
	planar::boundary_buffer_view const &_boundary,
	planar::simulation::stam::pressure_buffer_view const &_pressure)
{
	FCPPT_ASSERT_PRE(
		_velocity.get().size() == _boundary.get().size());

	FCPPT_ASSERT_PRE(
		_pressure.get().size() == _boundary.get().size());

	kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	kernel_->buffer_argument(
		"velocity",
		_velocity.get().buffer());

	kernel_->buffer_argument(
		"pressure",
		_pressure.get().buffer());

	kernel_->numerical_argument(
		"buffer_pitch",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_boundary.get().size().w())));

	kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim2(
			_boundary.get().size()));
}

flakelib::planar::simulation::stam::subtract_pressure_gradient::~subtract_pressure_gradient()
{
}
