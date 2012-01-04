#ifndef FLAKELIB_PLANAR_FRAMEWORK_HPP_INCLUDED
#define FLAKELIB_PLANAR_FRAMEWORK_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/planar/boundary_view.hpp>
#include <flakelib/planar/density/advector.hpp>
#include <flakelib/planar/density/cursor_splatter.hpp>
#include <flakelib/planar/density/monitor_proxy.hpp>
#include <flakelib/planar/monitor/parent.hpp>
#include <flakelib/planar/monitor/planar_arrows.hpp>
#include <flakelib/planar/monitor/planar_converter.hpp>
#include <flakelib/planar/monitor/texture.hpp>
#include <flakelib/planar/monitor/texture_fwd.hpp>
#include <flakelib/planar/simulation/base_fwd.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <rucksack/widget/master_and_slaves.hpp>
#include <rucksack/widget/viewport_adaptor.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/input/cursor/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/sprite/external_system_impl.hpp>
#include <sge/sprite/object.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace planar
{
class framework
{
FCPPT_NONCOPYABLE(
	framework);
public:
	explicit
	framework(
		sge::viewport::manager &,
		sge::renderer::device &,
		sge::opencl::command_queue::object &,
		simulation::base &,
		sge::font::system &,
		flakelib::build_options const &,
		flakelib::planar::boundary_view const &,
		sge::parse::json::object const &,
		sge::input::cursor::object &,
		buffer_pool::object &,
		utility::object &);

	void
	update(
		flakelib::duration const &);

	void
	render();

	sge::renderer::state::list const
	render_states() const;

	~framework();
private:
	typedef
	boost::ptr_map<fcppt::string,monitor::texture>
	additional_data_monitors;

	sge::renderer::device &renderer_;
	simulation::base &simulation_;
	monitor::parent monitor_parent_;
	monitor::planar_converter planar_converter_;
	rucksack::widget::viewport_adaptor viewport_widget_;
	rucksack::widget::master_and_slaves master_and_slaves_;
	rucksack::widget::box::base master_box_;
	monitor::planar_arrows velocity_arrows_;
	density::advector density_advector_;
	density::cursor_splatter density_cursor_splatter_;
	density::monitor_proxy density_monitor_;
	additional_data_monitors additional_data_;
};
}
}

#endif
