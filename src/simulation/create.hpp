#ifndef FLAKE_SIMULATION_CREATE_HPP_INCLUDED
#define FLAKE_SIMULATION_CREATE_HPP_INCLUDED

#include "base_ptr.hpp"
#include "../boundary_view.hpp"
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>

namespace flake
{
namespace simulation
{
flake::simulation::base_ptr
create(
	sge::opencl::context::object &,
	sge::opencl::command_queue::object &,
	flake::boundary_view const &,
	sge::parse::json::object const &);
}
}

#endif
