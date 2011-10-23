#ifndef FLAKELIB_SIMULATION_CREATE_HPP_INCLUDED
#define FLAKELIB_SIMULATION_CREATE_HPP_INCLUDED

#include <flakelib/boundary_view.hpp>
#include <flakelib/simulation/base_ptr.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>


namespace flakelib
{
namespace simulation
{
flakelib::simulation::base_ptr
create(
	sge::opencl::context::object &,
	sge::opencl::command_queue::object &,
	flakelib::boundary_view const &,
	sge::parse::json::object const &);
}
}

#endif
