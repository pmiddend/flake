#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_OBJECT_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_OBJECT_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/buffer_pool/volume_lock.hpp>
#include <flakelib/profiler/object.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <flakelib/volume/boundary/view.hpp>
#include <flakelib/volume/laplace_solver/base_fwd.hpp>
#include <flakelib/volume/simulation/stam/backward_advected.hpp>
#include <flakelib/volume/simulation/stam/external_force_magnitude.hpp>
#include <flakelib/volume/simulation/stam/forward_advected.hpp>
#include <flakelib/volume/simulation/stam/pressure.hpp>
#include <flakelib/volume/simulation/stam/profiling_enabled.hpp>
#include <flakelib/volume/simulation/stam/rhs.hpp>
#include <flakelib/volume/simulation/stam/solution.hpp>
#include <flakelib/volume/simulation/stam/vector_field.hpp>
#include <flakelib/volume/simulation/stam/velocity.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/unique_ptr.hpp>


namespace flakelib
{
namespace volume
{
namespace simulation
{
namespace stam
{
/**
\brief This class simulates a liquid originating from a slit at the left

It displays the resulting vector field at the left (with the boundary in the
background), as well as helper images at the right.

Parameters are given to the class via a json object.
*/
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::command_queue::object &,
		volume::boundary::view const &,
		stam::external_force_magnitude const &,
		stam::profiling_enabled const &,
		flakelib::build_options const &,
		buffer_pool::object &,
		utility::object &,
		volume::laplace_solver::base &);

	// @override
	buffer::volume_view<cl_float4> const
	velocity();

	profiler::object const &
	parent_profiler() const;

	void
	update(
		flakelib::duration const &);

	void
	external_force_magnitude(
		cl_float);

	~object();
private:
	typedef
	flakelib::buffer_pool::volume_lock<cl_float>
	volume_float_lock;

	typedef
	flakelib::buffer_pool::volume_lock<cl_float4>
	volume_float4_lock;

	typedef
	fcppt::unique_ptr<volume_float_lock>
	unique_volume_float_lock;

	typedef
	fcppt::unique_ptr<volume_float4_lock>
	unique_volume_float4_lock;

	typedef
	flakelib::buffer::volume_view<cl_float>
	volume_float_view;

	typedef
	flakelib::buffer::volume_view<cl_float4>
	volume_float4_view;

	sge::opencl::command_queue::object &command_queue_;
	utility::object &utility_;
	buffer_pool::object &buffer_pool_;
	volume::laplace_solver::base &laplace_solver_;
	cl_float external_force_magnitude_;
	bool const profiling_enabled_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object advect_kernel_;
	sge::opencl::kernel::object apply_external_forces_kernel_;
	sge::opencl::kernel::object divergence_kernel_;
	sge::opencl::kernel::object gradient_and_subtract_kernel_;
	sge::opencl::kernel::object maccormack_kernel_;
	flakelib::profiler::object parent_profiler_;
	flakelib::profiler::object advection_profiler_;
	flakelib::profiler::object external_forces_profiler_;
	flakelib::profiler::object divergence_profiler_;
	flakelib::profiler::object project_profiler_;
	flakelib::profiler::object solve_profiler_;
	volume::boundary::view boundary_;
	unique_volume_float4_lock velocity_image_;
	unique_volume_float_lock divergence_image_;
	unique_volume_float_lock vector_magnitude_image_;
	unique_volume_float_lock residual_image_;
	unique_volume_float_lock pressure_image_;

	unique_volume_float4_lock
	advect(
		volume_float4_view const &,
		flakelib::duration const &);

	void
	apply_forces(
		volume_float4_view const &);

	unique_volume_float_lock
	divergence(
		volume_float4_view const &);

	unique_volume_float_lock
	solve(
		volume_float_view const &);

	unique_volume_float4_lock
	gradient_and_subtract(
		stam::vector_field const &,
		stam::pressure const &);

	unique_volume_float4_lock
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
