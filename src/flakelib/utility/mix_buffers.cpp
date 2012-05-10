#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/utility/mix_buffers.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::utility::mix_buffers::mix_buffers(
	cl::program_context const &_program_context)
:
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/utility/mix_buffers.cl")),
		_program_context.compiler_flags()),
	add_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"mix_buffers_apply_add")))
{
}

void
flakelib::utility::mix_buffers::add_from_to(
	buffer::linear_view<cl_float> const &_from_buffer,
	buffer::linear_view<cl_float> const &_to_buffer)
{
	FCPPT_ASSERT_PRE(
		_from_buffer.size() == _to_buffer.size());

	add_kernel_->buffer_argument(
		"from",
		_from_buffer.buffer());

	add_kernel_->buffer_argument(
		"to",
		_to_buffer.buffer());

	add_kernel_->enqueue_automatic(
		flakelib::cl::global_dim1(
			_from_buffer.size()));
}

flakelib::utility::mix_buffers::~mix_buffers()
{
}
