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
	sge::opencl::memory_object::image::planar p_;
	sge::opencl::memory_object::image::planar boundary_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object reset_vector_field_;
	sge::opencl::kernel::object copy_boundary_;
};
}
}

#endif
