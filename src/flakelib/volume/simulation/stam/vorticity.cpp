#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/simulation/stam/vorticity.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/move.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::volume::simulation::stam::vorticity::vorticity(
	cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool)
:
	buffer_pool_(
		_buffer_pool),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/volume/vorticity.cl")),
		_program_context.compiler_flags()),
	vorticity_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"vorticity_apply"))),
	confinement_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"vorticity_gradient_and_cross")))
{
}

flakelib::volume::unique_float4_buffer_lock
flakelib::volume::simulation::stam::vorticity::apply_vorticity(
	volume::boundary_buffer_view const &_boundary,
	stam::velocity const &_velocity)
{
	FCPPT_ASSERT_PRE(
		_velocity.get().size() == _boundary.get().size());

	flakelib::volume::unique_float4_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float4_buffer_lock>(
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

flakelib::volume::unique_float4_buffer_lock
flakelib::volume::simulation::stam::vorticity::apply_confinement(
	volume::float4_view const &_vorticity,
	stam::velocity const &_velocity,
	flakelib::duration const &_dt,
	stam::vorticity_strength const &_strength)
{
	flakelib::volume::unique_float4_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float4_buffer_lock>(
			fcppt::ref(
				buffer_pool_),
			_vorticity.size()));

	confinement_kernel_->numerical_argument(
		"vorticity_strength",
		_strength.get());

	confinement_kernel_->numerical_argument(
		"dt",
		static_cast<cl_float>(
			_dt.count()));

	confinement_kernel_->buffer_argument(
		"vorticity",
		_vorticity.buffer());

	confinement_kernel_->buffer_argument(
		"velocity",
		_velocity.get().buffer());

	confinement_kernel_->buffer_argument(
		"output",
		result->value().buffer());

	confinement_kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_vorticity.size().w()));

	confinement_kernel_->enqueue_automatic(
		_vorticity.size());

	return
		fcppt::move(
			result);
}

flakelib::volume::simulation::stam::vorticity::~vorticity()
{
}
