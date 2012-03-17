#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/buffer_pool/planar_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/planar/simulation/stam/vorticity.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/move.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::planar::simulation::stam::vorticity::vorticity(
	cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool)
:
	buffer_pool_(
		_buffer_pool),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/planar/vorticity.cl")),
		_program_context.compiler_flags()),
	vorticity_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"apply_vorticity")))
{
}

flakelib::planar::unique_float_buffer_lock
flakelib::planar::simulation::stam::vorticity::apply_vorticity(
	planar::boundary_buffer_view const &_boundary,
	stam::velocity const &_velocity)
{
	FCPPT_ASSERT_PRE(
		_velocity.get().size() == _boundary.get().size());

	flakelib::planar::unique_float_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::planar::float_buffer_lock>(
			fcppt::ref(
				buffer_pool_),
			_velocity.get().size()));

	vorticity_kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	vorticity_kernel_->buffer_argument(
		"velocity",
		_velocity.get().buffer());

	vorticity_kernel_->buffer_argument(
		"output",
		result->value().buffer());

	vorticity_kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_velocity.get().size().w()));

	vorticity_kernel_->enqueue_automatic(
		_velocity.get().size());

	return
		fcppt::move(
			result);
}

flakelib::planar::simulation::stam::vorticity::~vorticity()
{
}
