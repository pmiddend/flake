#ifndef FLAKELIB_SIMULATION_STAM_OBJECT_HPP_INCLUDED
#define FLAKELIB_SIMULATION_STAM_OBJECT_HPP_INCLUDED

#include <flakelib/additional_planar_data.hpp>
#include <flakelib/build_options.hpp>
#include <flakelib/planar_boundary_view.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/buffer_pool/planar_lock.hpp>
#include <flakelib/laplace_solver/planar/base_fwd.hpp>
#include <flakelib/profiler/object.hpp>
#include <flakelib/simulation/base.hpp>
#include <flakelib/simulation/stam/pressure.hpp>
#include <flakelib/simulation/stam/rhs.hpp>
#include <flakelib/simulation/stam/solution.hpp>
#include <flakelib/simulation/stam/vector_field.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <fcppt/unique_ptr.hpp>


namespace flakelib
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
:
	public simulation::base
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::command_queue::object &,
		flakelib::planar_boundary_view const &,
		sge::parse::json::object const &,
		flakelib::build_options const &,
		buffer_pool::object &,
		utility::object &,
		laplace_solver::planar::base &);

	// @override
	buffer::planar_view<cl_float2> const
	velocity();

	// @override
	flakelib::additional_planar_data const &
	additional_planar_data() const;

	profiler::object const &
	parent_profiler() const;

	void
	update(
		flakelib::duration const &);

	~object();
private:
	typedef
	flakelib::buffer_pool::planar_lock<cl_float>
	planar_float_lock;

	typedef
	flakelib::buffer_pool::planar_lock<cl_float2>
	planar_float2_lock;

	typedef
	fcppt::unique_ptr<planar_float_lock>
	unique_planar_float_lock;

	typedef
	fcppt::unique_ptr<planar_float2_lock>
	unique_planar_float2_lock;

	typedef
	flakelib::buffer::planar_view<cl_float>
	planar_float_view;

	typedef
	flakelib::buffer::planar_view<cl_float2>
	planar_float2_view;

	sge::opencl::command_queue::object &command_queue_;
	utility::object &utility_;
	buffer_pool::object &buffer_cache_;
	laplace_solver::planar::base &laplace_solver_;
	cl_float const external_force_magnitude_;
	cl_float const grid_scale_;
	bool const profiling_enabled_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object advect_kernel_;
	sge::opencl::kernel::object apply_external_forces_kernel_;
	sge::opencl::kernel::object divergence_kernel_;
	sge::opencl::kernel::object laplacian_residual_absolute_value_kernel_;
	sge::opencl::kernel::object gradient_and_subtract_kernel_;
	sge::opencl::kernel::object gradient_kernel_;
	flakelib::profiler::object parent_profiler_;
	flakelib::profiler::object advection_profiler_;
	flakelib::profiler::object external_forces_profiler_;
	flakelib::profiler::object divergence_profiler_;
	flakelib::profiler::object project_profiler_;
	flakelib::profiler::object solve_profiler_;
	mutable flakelib::additional_planar_data additional_planar_data_;
	planar_float_lock boundary_image_;
	unique_planar_float2_lock velocity_image_,old_velocity_image_,gradient_image_;
	unique_planar_float_lock divergence_image_;
	unique_planar_float_lock vector_magnitude_image_;
	unique_planar_float_lock residual_image_;
	unique_planar_float_lock pressure_image_;

	unique_planar_float2_lock
	advect(
		planar_float2_view const &,
		flakelib::duration const &);

	void
	apply_forces(
		planar_float2_view const &);

	unique_planar_float_lock
	divergence(
		planar_float2_view const &);

	unique_planar_float_lock
	solve(
		planar_float_view const &);

	unique_planar_float2_lock
	gradient_and_subtract(
		stam::vector_field const &,
		stam::pressure const &);

	unique_planar_float_lock
	laplacian_residual(
		stam::solution const &,
		stam::rhs const &);

	unique_planar_float_lock
	vector_magnitude(
		planar_float2_view const &);

	unique_planar_float2_lock
	gradient(
		planar_float_view const &);
};
}
}
}

#endif
