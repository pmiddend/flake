#ifndef FLAKELIB_VISUALIZATION_CREATE_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_CREATE_HPP_INCLUDED

#include <flakelib/visualization/base_ptr.hpp>
#include <flakelib/boundary_view.hpp>
#include <flakelib/simulation/base_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>

namespace flakelib
{
namespace visualization
{
flakelib::visualization::base_ptr
create(
	sge::renderer::device &,
	sge::opencl::context::object &,
	sge::opencl::command_queue::object &,
	simulation::base &,
	flakelib::boundary_view const &,
	sge::parse::json::object const &);
}
}

#endif
