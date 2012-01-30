#ifndef FLAKELIB_PLANAR_FRAMEWORK_HPP_INCLUDED
#define FLAKELIB_PLANAR_FRAMEWORK_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/planar/laplace_solver/dynamic_factory_fwd.hpp>
#include <flakelib/planar/boundary_view.hpp>
#include <flakelib/planar/density/advector_fwd.hpp>
#include <flakelib/planar/density/cursor_splatter_fwd.hpp>
#include <flakelib/planar/density/monitor_proxy_fwd.hpp>
#include <flakelib/planar/buoyancy/object_fwd.hpp>
#include <flakelib/planar/monitor/parent_fwd.hpp>
#include <flakelib/planar/monitor/planar_arrows_fwd.hpp>
#include <flakelib/planar/monitor/planar_converter_fwd.hpp>
#include <flakelib/planar/monitor/texture_fwd.hpp>
#include <flakelib/planar/simulation/base_fwd.hpp>
#include <flakelib/planar/simulation/stam/wind_source_fwd.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <rucksack/widget/master_and_slaves_fwd.hpp>
#include <rucksack/widget/viewport_adaptor_fwd.hpp>
#include <rucksack/widget/box/base_fwd.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/input/cursor/object_fwd.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/unique_ptr.hpp>
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
	fcppt::unique_ptr<laplace_solver::dynamic_factory> solver_factory_;
	fcppt::unique_ptr<simulation::base> simulation_;
	fcppt::unique_ptr<simulation::stam::wind_source> wind_source_;
	fcppt::unique_ptr<monitor::parent> monitor_parent_;
	fcppt::unique_ptr<monitor::planar_converter> planar_converter_;
	fcppt::unique_ptr<rucksack::widget::viewport_adaptor> viewport_widget_;
	fcppt::unique_ptr<rucksack::widget::master_and_slaves> master_and_slave_;
	fcppt::unique_ptr<rucksack::widget::box::base> master_box_;
	fcppt::unique_ptr<monitor::planar_arrows> velocity_arrows_;
	fcppt::unique_ptr<density::advector> density_advector_;
	fcppt::unique_ptr<density::cursor_splatter> density_cursor_splatter_;
	fcppt::unique_ptr<density::monitor_proxy> density_monitor_;
	fcppt::unique_ptr<density::advector> temperature_advector_;
	fcppt::unique_ptr<density::cursor_splatter> temperature_cursor_splatter_;
	fcppt::unique_ptr<density::monitor_proxy> temperature_monitor_;
	fcppt::unique_ptr<buoyancy::object> buoyancy_;
	additional_data_monitors additional_data_;
};
}
}

#endif
