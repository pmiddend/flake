#include <flakelib/simulation/create.hpp>
#include <flakelib/simulation/stam.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <fcppt/cref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>


flakelib::simulation::base_ptr
flakelib::simulation::create(
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_queue,
	flakelib::boundary_view const &_boundary,
	sge::parse::json::object const &_config_file)
{
	return
		flakelib::simulation::base_ptr(
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
