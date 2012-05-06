#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/simulation/stam/outflow_boundaries.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/text.hpp>


flakelib::volume::simulation::stam::outflow_boundaries::outflow_boundaries(
	cl::program_context const &_program_context)
:
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/volume/outflow_boundaries.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"outflow_boundaries_apply")))
{
}

void
flakelib::volume::simulation::stam::outflow_boundaries::update(
	volume::float4_view const &_view)
{
	kernel_->buffer_argument(
		"input",
		_view.buffer());

	kernel_->numerical_argument(
		"buffer_width",
		static_cast<cl_int>(
			_view.size().w()));

	kernel_->numerical_argument(
		"buffer_height",
		static_cast<cl_int>(
			_view.size().h()));

	kernel_->numerical_argument(
		"buffer_depth",
		static_cast<cl_int>(
			_view.size().d()));

	kernel_->numerical_argument(
		"line_pitch",
		static_cast<cl_uint>(
			_view.size().w()));

	kernel_->enqueue_automatic(
		sge::opencl::memory_object::dim2(
			_view.size().w(),
			_view.size().h()));
}

flakelib::volume::simulation::stam::outflow_boundaries::~outflow_boundaries()
{
}
