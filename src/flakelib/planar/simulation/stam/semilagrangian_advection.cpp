#include <flakelib/buffer_pool/planar_lock_impl.hpp>
#include <fcppt/chrono/duration_impl.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/assert/pre.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <flakelib/planar/simulation/stam/semilagrangian_advection.hpp>
#include <flakelib/program/create_from_context.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/cl/process_planar_buffer.hpp>
#include <fcppt/move.hpp>

flakelib::planar::simulation::stam::semilagrangian_advection::semilagrangian_advection(
	program::context const &_context,
	flakelib::buffer_pool::object &_buffer_pool)
:
	buffer_pool_(
		_buffer_pool),
	program_(
		flakelib::program::create_from_context(
			_context,
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/planar/semilagrangian_advection.cl")))),
	advect_kernel_(
		*program_,
		sge::opencl::kernel::name(
			"apply"))
{
}

flakelib::planar::unique_float2_buffer_lock
flakelib::planar::simulation::stam::semilagrangian_advection::update(
	planar::boundary_buffer_view const &_boundary,
	planar::float2_view const &_buffer,
	flakelib::duration const &_dt)
{
	FCPPT_ASSERT_PRE(
		_buffer.size() == _boundary.get().size());

	flakelib::planar::unique_float2_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::planar::float2_buffer_lock>(
			fcppt::ref(
				buffer_pool_),
			_buffer.size()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_boundary.get().buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_buffer.buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		result->value().buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_uint>(
			_buffer.size().w()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		static_cast<cl_float>(
			_dt.count()));

	flakelib::cl::process_planar_buffer(
		command_queue_,
		advect_kernel_,
		_buffer.size());

	return
		fcppt::move(
			result);
}

flakelib::planar::simulation::stam::semilagrangian_advection::~semilagrangian_advection()
{
}
