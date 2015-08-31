#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/simulation/stam/semilagrangian_advection.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/make_unique_ptr_fcppt.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::volume::simulation::stam::semilagrangian_advection::semilagrangian_advection(
	cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool)
:
	buffer_pool_(
		_buffer_pool),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/volume/semilagrangian_advection.cl")),
		_program_context.compiler_flags()),
	volume_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"semilagrangian_advection_apply_float4"))),
	scalar_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"semilagrangian_advection_apply_float")))
{
}

flakelib::volume::unique_float4_buffer_lock
flakelib::volume::simulation::stam::semilagrangian_advection::update_float4(
	volume::boundary_buffer_view const &_boundary,
	stam::velocity const &_velocity,
	volume::float4_view const &_buffer,
	flakelib::duration const &_dt)
{
	FCPPT_ASSERT_PRE(
		_velocity.get().size() == _boundary.get().size());

	FCPPT_ASSERT_PRE(
		_buffer.size() == _boundary.get().size());

	flakelib::volume::unique_float4_buffer_lock result(
		fcppt::make_unique_ptr_fcppt<flakelib::volume::float4_buffer_lock>(
			buffer_pool_,
			_buffer.size()));

	volume_kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	volume_kernel_->buffer_argument(
		"input",
		_buffer.buffer());

	volume_kernel_->buffer_argument(
		"velocity",
		_velocity.get().buffer());

	volume_kernel_->buffer_argument(
		"output",
		result->value().buffer());

	volume_kernel_->numerical_argument(
		"buffer_pitch",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_buffer.size().w())));

	volume_kernel_->numerical_argument(
		"dt",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_float>(
				_dt.count())));

	volume_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim3(
			_buffer.size()));

	return
		result;
}

flakelib::volume::unique_float_buffer_lock
flakelib::volume::simulation::stam::semilagrangian_advection::update_float(
	volume::boundary_buffer_view const &_boundary,
	stam::velocity const &_velocity,
	volume::float_view const &_buffer,
	flakelib::duration const &_dt)
{
	FCPPT_ASSERT_PRE(
		_velocity.get().size() == _boundary.get().size());

	FCPPT_ASSERT_PRE(
		_buffer.size() == _boundary.get().size());

	flakelib::volume::unique_float_buffer_lock result(
		fcppt::make_unique_ptr_fcppt<flakelib::volume::float_buffer_lock>(
			buffer_pool_,
			_buffer.size()));

	scalar_kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	scalar_kernel_->buffer_argument(
		"input",
		_buffer.buffer());

	scalar_kernel_->buffer_argument(
		"velocity",
		_velocity.get().buffer());

	scalar_kernel_->buffer_argument(
		"output",
		result->value().buffer());

	scalar_kernel_->numerical_argument(
		"buffer_pitch",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_buffer.size().w())));

	scalar_kernel_->numerical_argument(
		"dt",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_float>(
				_dt.count())));

	scalar_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim3(
			_buffer.size()));

	return
		result;
}

flakelib::volume::simulation::stam::semilagrangian_advection::~semilagrangian_advection()
{
}
