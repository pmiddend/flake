#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/buffer_pool/planar_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/planar/simulation/stam/semilagrangian_advection.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/config/external_begin.hpp>
#include <utility>
#include <fcppt/config/external_end.hpp>


flakelib::planar::simulation::stam::semilagrangian_advection::semilagrangian_advection(
	cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool)
:
	buffer_pool_(
		_buffer_pool),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/planar/semilagrangian_advection.cl")),
		_program_context.compiler_flags()),
	planar_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"semilagrangian_advection_apply_float2"))),
	scalar_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"semilagrangian_advection_apply_float")))
{
}

flakelib::planar::unique_float2_buffer_lock
flakelib::planar::simulation::stam::semilagrangian_advection::update_float2(
	planar::boundary_buffer_view const &_boundary,
	stam::velocity const &_velocity,
	planar::float2_view const &_buffer,
	flakelib::duration const &_dt)
{
	FCPPT_ASSERT_PRE(
		_velocity.get().size() == _boundary.get().size());

	FCPPT_ASSERT_PRE(
		_buffer.size() == _boundary.get().size());

	flakelib::planar::unique_float2_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::planar::float2_buffer_lock>(
			buffer_pool_,
			_buffer.size()));

	planar_kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	planar_kernel_->buffer_argument(
		"input",
		_buffer.buffer());

	planar_kernel_->buffer_argument(
		"velocity",
		_velocity.get().buffer());

	planar_kernel_->buffer_argument(
		"output",
		result->value().buffer());

	planar_kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_buffer.size().w()));

	planar_kernel_->numerical_argument(
		"dt",
		static_cast<cl_float>(
			_dt.count()));

	planar_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim2(
			_buffer.size()));

	return
		std::move(
			result);
}

flakelib::planar::unique_float_buffer_lock
flakelib::planar::simulation::stam::semilagrangian_advection::update_float(
	planar::boundary_buffer_view const &_boundary,
	stam::velocity const &_velocity,
	planar::float_view const &_buffer,
	flakelib::duration const &_dt)
{
	FCPPT_ASSERT_PRE(
		_velocity.get().size() == _boundary.get().size());

	FCPPT_ASSERT_PRE(
		_buffer.size() == _boundary.get().size());

	flakelib::planar::unique_float_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::planar::float_buffer_lock>(
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
		static_cast<cl_uint>(
			_buffer.size().w()));

	scalar_kernel_->numerical_argument(
		"dt",
		static_cast<cl_float>(
			_dt.count()));

	scalar_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim2(
			_buffer.size()));

	return
		std::move(
			result);
}

flakelib::planar::simulation::stam::semilagrangian_advection::~semilagrangian_advection()
{
}
