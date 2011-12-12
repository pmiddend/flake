#ifndef FLAKELIB_ARROW_VISUAL_HPP_INCLUDED
#define FLAKELIB_ARROW_VISUAL_HPP_INCLUDED

#include <flakelib/boundary_view.hpp>
#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/monitor/parent.hpp>
#include <flakelib/monitor/planar_arrows.hpp>
#include <flakelib/monitor/planar_converter.hpp>
#include <flakelib/simulation/base_fwd.hpp>
#include <rucksack/widget/viewport_adaptor.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
class arrow_visual
{
FCPPT_NONCOPYABLE(
	arrow_visual);
public:
	explicit
	arrow_visual(
		simulation::base &,
		sge::renderer::device &_renderer,
		sge::opencl::command_queue::object &_command_queue,
		sge::font::system &_font_system,
		sge::parse::json::object const &_config_file,
		flakelib::build_options const &_build_options,
		sge::viewport::manager &_viewport_manager,
		flakelib::boundary_view const &_boundary);

	void
	update(
		flakelib::duration const &);

	void
	render();

	~arrow_visual();
private:
	simulation::base &simulation_;
	monitor::parent monitor_parent_;
	monitor::planar_converter planar_converter_;
	rucksack::widget::viewport_adaptor viewport_widget_;
	monitor::planar_arrows velocity_arrows_;
};
}

#endif
