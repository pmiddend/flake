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
				"vorticity_apply"))),
	confinement_data_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"vorticity_confinement_data"))),
	confinement_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"vorticity_gradient_and_cross")))
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
			buffer_pool_,
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
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_velocity.get().size().w())));

	vorticity_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim2(
			_velocity.get().size()));

	return
		result;
}

flakelib::planar::unique_float2_buffer_lock
flakelib::planar::simulation::stam::vorticity::confinement_data(
	planar::float_view const &_vorticity,
	flakelib::duration const &_dt,
	stam::vorticity_strength const &_strength)
{
	flakelib::planar::unique_float2_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::planar::float2_buffer_lock>(
			buffer_pool_,
			_vorticity.size()));

	confinement_data_kernel_->numerical_argument(
		"vorticity_strength",
		sge::opencl::kernel::numeric_type(
			_strength.get()));

	confinement_data_kernel_->numerical_argument(
		"dt",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_float>(
				_dt.count())));

	confinement_data_kernel_->buffer_argument(
		"vorticity",
		_vorticity.buffer());

	confinement_data_kernel_->buffer_argument(
		"output",
		result->value().buffer());

	confinement_data_kernel_->numerical_argument(
		"buffer_pitch",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_vorticity.size().w())));

	confinement_data_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim2(
			_vorticity.size()));

	return
		result;
}

flakelib::planar::unique_float2_buffer_lock
flakelib::planar::simulation::stam::vorticity::apply_confinement(
	planar::float_view const &_vorticity,
	stam::velocity const &_velocity,
	flakelib::duration const &_dt,
	stam::vorticity_strength const &_strength)
{
	flakelib::planar::unique_float2_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::planar::float2_buffer_lock>(
			buffer_pool_,
			_vorticity.size()));

	confinement_kernel_->numerical_argument(
		"vorticity_strength",
		sge::opencl::kernel::numeric_type(
			_strength.get()));

	confinement_kernel_->numerical_argument(
		"dt",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_float>(
				_dt.count())));

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
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_vorticity.size().w())));

	confinement_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim2(
			_vorticity.size()));

	return
		result;
}

flakelib::planar::simulation::stam::vorticity::~vorticity()
{
}
