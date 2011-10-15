#ifndef FLAKELIB_SIMULATION_STAM_HPP_INCLUDED
#define FLAKELIB_SIMULATION_STAM_HPP_INCLUDED

#include <flakelib/boundary_view.hpp>
#include <flakelib/profiler/object.hpp>
#include <flakelib/simulation/base.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <CL/cl.h>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace simulation
{
class stam
:
	public simulation::base
{
FCPPT_NONCOPYABLE(
	stam);
public:
	explicit
	stam(
		sge::opencl::context::object &,
		sge::opencl::command_queue::object &,
		flakelib::boundary_view const &,
		sge::parse::json::object const &);

	flakelib::planar_object const
	velocity();

	flakelib::additional_planar_data const &
	additional_planar_data() const;

	void
	update(
		flakelib::duration const &);

	~stam();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::memory_object::image::planar v1_,v2_;
	sge::opencl::memory_object::image::planar divergence_;
	sge::opencl::memory_object::image::planar p1_,p2_,vector_magnitude_,residual_;
	sge::opencl::memory_object::image::planar boundary_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object null_image_kernel_;
	sge::opencl::kernel::object advect_kernel_;
	sge::opencl::kernel::object apply_external_forces_kernel_;
	sge::opencl::kernel::object divergence_kernel_;
	sge::opencl::kernel::object jacobi_kernel_;
	sge::opencl::kernel::object laplace_residual_kernel_;
	sge::opencl::kernel::object gradient_and_subtract_kernel_;
	sge::opencl::kernel::object copy_image_kernel_;
	sge::opencl::kernel::object vector_magnitude_kernel_;
	cl_float const external_force_magnitude_;
	cl_float const grid_scale_;
	cl_float const velocity_magnitude_scale_;
	unsigned const jacobi_iterations_;
	bool const profiling_enabled_;
	flakelib::profiler::object parent_profiler_;
	flakelib::profiler::object null_image_profiler_;
	flakelib::profiler::object advection_profiler_;
	flakelib::profiler::object external_forces_profiler_;
	flakelib::profiler::object divergence_profiler_;
	flakelib::profiler::object jacobi_profiler_;
	flakelib::profiler::object project_profiler_;
	flakelib::additional_planar_data additional_planar_data_;

	void
	null_image(
		sge::opencl::memory_object::image::planar &);

	void
	advect(
		flakelib::duration const &,
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to);

	void
	apply_forces(
		sge::opencl::memory_object::image::planar &,
		sge::opencl::memory_object::image::planar &,
		flakelib::duration const &);

	void
	divergence(
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to);

	void
	project(
		sge::opencl::memory_object::image::planar &v,
		sge::opencl::memory_object::image::planar &divergence,
		sge::opencl::memory_object::image::planar &target);

	void
	copy_image(
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to);

	void
	vector_magnitude(
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to,
		cl_float);

	void
	laplace_residual(
		sge::opencl::memory_object::image::planar &lhs,
		sge::opencl::memory_object::image::planar &rhs,
		sge::opencl::memory_object::image::planar &to);
};
}
}

#endif
