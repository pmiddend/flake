#ifndef FLAKE_SIMULATION_STAM_HPP_INCLUDED
#define FLAKE_SIMULATION_STAM_HPP_INCLUDED

#include "../boundary_view.hpp"
#include "base.hpp"
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

namespace flake
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
		flake::boundary_view const &,
		sge::parse::json::object const &);

	sge::opencl::memory_object::image::planar &
	vector_field();

	void
	update(
		flake::duration const &);

	~stam();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::memory_object::image::planar v1_,v2_;
	sge::opencl::memory_object::image::planar temporary_v_;
	sge::opencl::memory_object::image::planar p1_,p2_;
	sge::opencl::memory_object::image::planar boundary_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object null_image_;
	sge::opencl::kernel::object copy_boundary_;
	sge::opencl::kernel::object advect_;
	sge::opencl::kernel::object apply_external_forces_;
	sge::opencl::kernel::object divergence_;
	sge::opencl::kernel::object jacobi_;
	sge::opencl::kernel::object gradient_and_subtract_;
	cl_float const external_force_magnitude_;
	cl_float const grid_size_;
	unsigned const jacobi_iterations_;

	void
	copy_boundary(
		sge::opencl::memory_object::image::planar &);
	
	void
	advect(
		flake::duration const &,
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to);
	
	void
	apply_forces(
		sge::opencl::memory_object::image::planar &);

	void
	divergence(
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to);
	
	void
	project(
		sge::opencl::memory_object::image::planar &v,
		sge::opencl::memory_object::image::planar &divergence,
		sge::opencl::memory_object::image::planar &target);
};
}
}

#endif
