#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/gradient.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/move.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::volume::gradient::gradient(
	cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool)
:
	buffer_pool_(
		_buffer_pool),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/volume/gradient.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"gradient_apply")))
{
}

flakelib::volume::unique_float4_buffer_lock
flakelib::volume::gradient::update(
	volume::float_view const &_buffer)
{
	flakelib::volume::unique_float4_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float4_buffer_lock>(
			fcppt::ref(
				buffer_pool_),
			_buffer.size()));

	kernel_->buffer_argument(
		"input",
		_buffer.buffer());

	kernel_->buffer_argument(
		"output",
		result->value().buffer());

	kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_buffer.size().w()));

	kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim3(
			_buffer.size()));

	return
		fcppt::move(
			result);
}

flakelib::volume::gradient::~gradient()
{
}
