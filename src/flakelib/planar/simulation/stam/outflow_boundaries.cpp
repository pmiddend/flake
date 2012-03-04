#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/planar/simulation/stam/outflow_boundaries.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/text.hpp>


flakelib::planar::simulation::stam::outflow_boundaries::outflow_boundaries(
	cl::program_context const &_program_context)
:
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/planar/outflow_boundaries.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"apply")))
{
}

void
flakelib::planar::simulation::stam::outflow_boundaries::update(
	planar::float2_view const &_view)
{
	kernel_->buffer_argument(
		"input",
		_view.buffer());

	kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_view.size().w()));

	kernel_->enqueue_automatic(
		_view.size());
}

flakelib::planar::simulation::stam::outflow_boundaries::~outflow_boundaries()
{
}
