#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_OBJECT_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_OBJECT_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/buffer_pool/planar_lock.hpp>
#include <flakelib/planar/additional_scalar_data.hpp>
#include <flakelib/planar/boundary_image_view.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/planar/float2_buffer_lock.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/float_buffer_lock.hpp>
#include <flakelib/planar/float_view.hpp>
#include <flakelib/planar/unique_float2_buffer_lock.hpp>
#include <flakelib/planar/unique_float_buffer_lock.hpp>
#include <flakelib/planar/laplace_solver/base_fwd.hpp>
#include <flakelib/planar/simulation/base.hpp>
#include <flakelib/planar/simulation/stam/backward_advected.hpp>
#include <flakelib/planar/simulation/stam/forward_advected.hpp>
#include <flakelib/planar/simulation/stam/pressure.hpp>
#include <flakelib/planar/simulation/stam/profiling_enabled.hpp>
#include <flakelib/planar/simulation/stam/rhs.hpp>
#include <flakelib/planar/simulation/stam/solution.hpp>
#include <flakelib/planar/simulation/stam/use_maccormack.hpp>
#include <flakelib/planar/simulation/stam/vector_field.hpp>
#include <flakelib/planar/simulation/stam/velocity.hpp>
#include <flakelib/profiler/object.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/unique_ptr.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
/**
\brief This class simulates a liquid originating from a slit at the left

It displays the resulting vector field at the left (with the boundary in the
background), as well as helper images at the right.
*/
class object
:
	public simulation::base
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::command_queue::object &,
		flakelib::planar::boundary_image_view const &,
		flakelib::build_options const &,
		buffer_pool::object &,
		utility::object &,
		planar::laplace_solver::base &,
		stam::profiling_enabled const &,
		stam::use_maccormack const &);

	// @override
	buffer::planar_view<cl_float2> const
	velocity();

	// @override
	flakelib::planar::additional_scalar_data const &
	additional_scalar_data() const;

	profiler::object const &
	parent_profiler() const;

	void
	update(
		flakelib::duration const &);

	flakelib::planar::boundary_buffer_view const
	boundary() const;

	~object();
private:
	sge::opencl::command_queue::object &command_queue_;
	utility::object &utility_;
	buffer_pool::object &buffer_cache_;
	planar::laplace_solver::base &laplace_solver_;
	bool const profiling_enabled_;
	bool const use_maccormack_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object advect_kernel_;
	sge::opencl::kernel::object divergence_kernel_;
	sge::opencl::kernel::object laplacian_residual_absolute_value_kernel_;
	sge::opencl::kernel::object gradient_and_subtract_kernel_;
	sge::opencl::kernel::object gradient_kernel_;
	sge::opencl::kernel::object maccormack_kernel_;
	flakelib::profiler::object parent_profiler_;
	flakelib::profiler::object advection_profiler_;
	flakelib::profiler::object divergence_profiler_;
	flakelib::profiler::object project_profiler_;
	flakelib::profiler::object solve_profiler_;
	mutable flakelib::planar::additional_scalar_data additional_scalar_data_;
	planar::float_buffer_lock boundary_buffer_;
	planar::unique_float2_buffer_lock velocity_image_,old_velocity_image_,gradient_image_;
	planar::unique_float_buffer_lock divergence_image_;
	planar::unique_float_buffer_lock vector_magnitude_image_;
	planar::unique_float_buffer_lock residual_image_;
	planar::unique_float_buffer_lock pressure_image_;

	planar::unique_float2_buffer_lock
	advect(
		planar::float2_view const &,
		flakelib::duration const &);

	planar::unique_float_buffer_lock
	divergence(
		planar::float2_view const &);

	planar::unique_float_buffer_lock
	solve(
		planar::float_view const &);

	planar::unique_float2_buffer_lock
	gradient_and_subtract(
		stam::vector_field const &,
		stam::pressure const &);

	planar::unique_float_buffer_lock
	laplacian_residual(
		stam::solution const &,
		stam::rhs const &);

	planar::unique_float_buffer_lock
	vector_magnitude(
		planar::float2_view const &);

	planar::unique_float2_buffer_lock
	gradient(
		planar::float_view const &);

	planar::unique_float2_buffer_lock
	maccormack(
		stam::forward_advected const &,
		stam::backward_advected const &,
		stam::velocity const &,
		flakelib::duration const &);
};
}
}
}
}

#endif
