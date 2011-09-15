#ifndef FLAKE_VISUALIZATION_CREATE_HPP_INCLUDED
#define FLAKE_VISUALIZATION_CREATE_HPP_INCLUDED

#include "base_ptr.hpp"
#include "../simulation/base_fwd.hpp"
#include <sge/renderer/device_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>

namespace flake
{
namespace visualization
{
flake::visualization::base_ptr
create(
	sge::opencl::context::object &,
	sge::opencl::command_queue::object &,
	simulation::base &,
	sge::renderer::device &,
	sge::parse::json::object const &);
}
}

#endif
