#ifndef FLAKELIB_PLANAR_ARROWS_ONLY_HPP_INCLUDED
#define FLAKELIB_PLANAR_ARROWS_ONLY_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/planar/boundary_view.hpp>
#include <flakelib/planar/monitor/parent.hpp>
#include <flakelib/planar/monitor/planar_arrows.hpp>
#include <flakelib/planar/monitor/planar_converter.hpp>
#include <flakelib/planar/monitor/texture.hpp>
#include <flakelib/planar/monitor/texture_fwd.hpp>
#include <flakelib/planar/simulation/base_fwd.hpp>
#include <rucksack/widget/viewport_adaptor.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace planar
{
class arrows_only
{
FCPPT_NONCOPYABLE(
	arrows_only);
public:
	explicit
	arrows_only(
		sge::viewport::manager &,
		sge::renderer::device &,
		sge::opencl::command_queue::object &,
		simulation::base &,
		sge::font::system &,
		flakelib::build_options const &,
		flakelib::planar::boundary_view const &,
		sge::parse::json::object const &);

	void
	update(
		flakelib::duration const &);

	void
	render(
		sge::renderer::matrix4 const &);

	sge::renderer::state::list const
	render_states() const;

	~arrows_only();
private:
	sge::renderer::device &renderer_;
	simulation::base &simulation_;
	monitor::parent monitor_parent_;
	monitor::planar_converter planar_converter_;
	rucksack::widget::viewport_adaptor viewport_widget_;
	rucksack::widget::box::base master_box_;
	monitor::planar_arrows velocity_arrows_;
};
}
}

#endif
