#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/utility/fill_buffer.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>


flakelib::utility::fill_buffer::fill_buffer(
	cl::program_context const &_program_context)
:
	program_(
		_program_context.log_context(),
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/utility/fill_buffer.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"fill_buffer_apply")))
{
}

void
flakelib::utility::fill_buffer::apply(
	buffer::linear_view<cl_float> const &_buffer,
	cl_float const _value)
{
	kernel_->buffer_argument(
		"input",
		_buffer.buffer());

	kernel_->numerical_argument(
		"value",
		sge::opencl::kernel::numeric_type(
			_value));

	kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim1(
			_buffer.size()));
}

flakelib::utility::fill_buffer::~fill_buffer()
{
}
