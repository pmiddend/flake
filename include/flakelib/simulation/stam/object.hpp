#ifndef FLAKELIB_SIMULATION_STAM_OBJECT_HPP_INCLUDED
#define FLAKELIB_SIMULATION_STAM_OBJECT_HPP_INCLUDED

#include <flakelib/additional_planar_data.hpp>
#include <flakelib/boundary_view.hpp>
#include <flakelib/laplace_solver/base_fwd.hpp>
#include <flakelib/planar_pool/unique_lock.hpp>
#include <flakelib/profiler/object.hpp>
#include <flakelib/simulation/arrow_image_cache.hpp>
#include <flakelib/simulation/base.hpp>
#include <flakelib/simulation/scalar_image_cache.hpp>
#include <flakelib/simulation/stam/pressure.hpp>
#include <flakelib/simulation/stam/rhs.hpp>
#include <flakelib/simulation/stam/solution.hpp>
#include <flakelib/simulation/stam/vector_field.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/parse/json/object_fwd.hpp>


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
		flakelib::boundary_view const &,
		sge::parse::json::object const &,
		simulation::arrow_image_cache const &,
		simulation::scalar_image_cache const &,
		utility::object &,
		laplace_solver::base &);

	// @override
	flakelib::planar_object const
	velocity();

	// @override
	flakelib::additional_planar_data const &
	additional_planar_data() const;

	void
	update(
		flakelib::duration const &);

	~object();
private:
	sge::opencl::command_queue::object &command_queue_;
	utility::object &utility_;
	flakelib::planar_pool::object &arrow_image_cache_;
	flakelib::planar_pool::object &scalar_image_cache_;
	laplace_solver::base &laplace_solver_;
	cl_float const external_force_magnitude_;
	cl_float const grid_scale_;
	bool const profiling_enabled_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object advect_kernel_;
	sge::opencl::kernel::object apply_external_forces_kernel_;
	sge::opencl::kernel::object divergence_kernel_;
	sge::opencl::kernel::object laplacian_residual_absolute_value_kernel_;
	sge::opencl::kernel::object gradient_and_subtract_kernel_;
	flakelib::profiler::object parent_profiler_;
	flakelib::profiler::object advection_profiler_;
	flakelib::profiler::object external_forces_profiler_;
	flakelib::profiler::object divergence_profiler_;
	flakelib::profiler::object project_profiler_;
	mutable flakelib::additional_planar_data additional_planar_data_;
	sge::opencl::memory_object::image::planar boundary_image_;
	flakelib::planar_pool::unique_lock velocity_image_;
	flakelib::planar_pool::unique_lock divergence_image_;
	flakelib::planar_pool::unique_lock vector_magnitude_image_;
	flakelib::planar_pool::unique_lock residual_image_;
	flakelib::planar_pool::unique_lock pressure_image_;

	planar_pool::unique_lock
	advect(
		sge::opencl::memory_object::image::planar &,
		flakelib::duration const &);

	planar_pool::unique_lock
	apply_forces(
		sge::opencl::memory_object::image::planar &,
		flakelib::duration const &);

	planar_pool::unique_lock
	divergence(
		sge::opencl::memory_object::image::planar &);

	planar_pool::unique_lock
	solve(
		sge::opencl::memory_object::image::planar &);

	planar_pool::unique_lock
	gradient_and_subtract(
		stam::vector_field const &,
		stam::pressure const &);

	planar_pool::unique_lock
	laplacian_residual(
		stam::solution const &,
		stam::rhs const &);

	planar_pool::unique_lock
	vector_magnitude(
		sge::opencl::memory_object::image::planar &);
};
}
}
}

#endif
