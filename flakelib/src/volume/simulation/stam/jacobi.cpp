#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/simulation/stam/jacobi.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <utility>
#include <fcppt/config/external_end.hpp>


flakelib::volume::simulation::stam::jacobi::jacobi(
	cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool,
	volume::simulation::stam::iterations const &_iterations)
:
	buffer_pool_(
		_buffer_pool),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/volume/jacobi.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"jacobi_apply"))),
	iterations_(
		_iterations.get() % 2u == 0u
		?
			_iterations+stam::iterations(1u)
		:
			_iterations)
{
}

flakelib::volume::unique_float_buffer_lock
flakelib::volume::simulation::stam::jacobi::update(
	volume::simulation::stam::initial_guess_buffer_view const &_initial_guess,
	volume::boundary_buffer_view const &_boundary,
	volume::simulation::stam::rhs_buffer_view const &_rhs)
{
	FCPPT_ASSERT_PRE(
		_initial_guess.get().size() == _boundary.get().size());

	FCPPT_ASSERT_PRE(
		_rhs.get().size() == _boundary.get().size());

	flakelib::volume::unique_float_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
			buffer_pool_,
			_boundary.get().size()));

	kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_boundary.get().size().w()));

	kernel_->buffer_argument(
		"rhs",
		_rhs.get().buffer());

	kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	sge::opencl::memory_object::buffer
		*current_source =
			&_initial_guess.get().buffer(),
		*current_destination =
			&result->value().buffer();

	for(stam::iterations i(0u); i < iterations_; ++i)
	{
		kernel_->buffer_argument(
			"input",
			*current_source);

		kernel_->buffer_argument(
			"output",
			*current_destination);

		std::swap(
			current_source,
			current_destination);

		kernel_->enqueue_automatic(
			sge::opencl::command_queue::global_dim3(
				_boundary.get().size()));
	}

	return
		std::move(
			result);
}

flakelib::volume::simulation::stam::jacobi::~jacobi()
{
}
