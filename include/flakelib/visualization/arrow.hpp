#ifndef FLAKELIB_VISUALIZATION_ARROW_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_ARROW_HPP_INCLUDED

#include <flakelib/boundary_view.hpp>
#include <flakelib/simulation/base_fwd.hpp>
#include <flakelib/visualization/base.hpp>
#include <flakelib/visualization/monitor/child.hpp>
#include <flakelib/visualization/monitor/parent.hpp>
#include <flakelib/visualization/monitor/planar_arrows.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/sprite/external_system_impl.hpp>
#include <sge/sprite/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace visualization
{
class arrow
:
	public visualization::base
{
FCPPT_NONCOPYABLE(
	arrow);
public:
	explicit
	arrow(
		sge::renderer::device &,
		sge::opencl::context::object &,
		sge::opencl::command_queue::object &,
		simulation::base &,
		sge::font::system &,
		flakelib::boundary_view const &,
		sge::parse::json::object const &);

	void
	update(
		flakelib::duration const &);

	void
	render();

	sge::renderer::state::list const
	render_states() const;

	~arrow();
private:
	typedef
	boost::ptr_map<fcppt::string,monitor::child>
	additional_data_monitors;

	sge::renderer::device &renderer_;
	simulation::base &simulation_;
	monitor::parent monitor_parent_;
	monitor::planar_arrows velocity_arrows_;
	additional_data_monitors additional_data_;
};
}
}

#endif
