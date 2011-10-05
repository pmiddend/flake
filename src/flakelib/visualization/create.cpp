#include <flakelib/visualization/create.hpp>
#include <flakelib/visualization/arrow.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/cref.hpp>

flakelib::visualization::base_ptr
flakelib::visualization::create(
	sge::renderer::device &_renderer,
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_queue,
	simulation::base &_simulation,
	flakelib::boundary_view const &_boundary,
	sge::parse::json::object const &_config_file)
{
	return
		flakelib::visualization::base_ptr(
			fcppt::make_unique_ptr<visualization::arrow>(
				fcppt::ref(
					_renderer),
				fcppt::ref(
					_context),
				fcppt::ref(
					_queue),
				fcppt::ref(
					_simulation),
				fcppt::cref(
					_boundary),
				fcppt::cref(
					_config_file)));
}
