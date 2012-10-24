#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/buffer_pool/planar_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/planar/simulation/stam/divergence.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/move.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::planar::simulation::stam::divergence::divergence(
	cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool)
:
	buffer_pool_(
		_buffer_pool),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/planar/divergence.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"divergence_apply")))
{
}

flakelib::planar::unique_float_buffer_lock
flakelib::planar::simulation::stam::divergence::update(
	planar::boundary_buffer_view const &_boundary,
	planar::float2_view const &_buffer)
{
	FCPPT_ASSERT_PRE(
		_buffer.size() == _boundary.get().size());

	flakelib::planar::unique_float_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::planar::float_buffer_lock>(
			fcppt::ref(
				buffer_pool_),
			_buffer.size()));

	kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

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
		sge::opencl::command_queue::global_dim2(
			_buffer.size()));

	return
		fcppt::move(
			result);
}

flakelib::planar::simulation::stam::divergence::~divergence()
{
}
