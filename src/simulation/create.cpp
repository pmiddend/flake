#include "create.hpp"
#include "stam.hpp"
#include <sge/image2d/view/const_object.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/cref.hpp>

flake::simulation::base_ptr
flake::simulation::create(
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_queue,
	flake::boundary_view const &_boundary,
	sge::parse::json::object const &_config_file)
{
	return
		flake::simulation::base_ptr(
			fcppt::make_unique_ptr<simulation::stam>(
				fcppt::ref(
					_context),
				fcppt::ref(
					_queue),
				fcppt::cref(
					_boundary),
				fcppt::cref(
					_config_file)));
}
