#ifndef FLAKELIB_VISUALIZATION_ARROW_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_ARROW_HPP_INCLUDED

#include <flakelib/visualization/base.hpp>
#include <flakelib/boundary_view.hpp>
#include <flakelib/visualization/monitor/parent.hpp>
#include <flakelib/visualization/monitor/planar_arrows.hpp>
#include <flakelib/visualization/monitor/texture.hpp>
#include <flakelib/simulation/base_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/sprite/object.hpp>
#include <sge/sprite/external_system_impl.hpp>
#include <fcppt/noncopyable.hpp>

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
	sge::renderer::device &renderer_;
	monitor::parent monitor_parent_;
	monitor::planar_arrows velocity_arrows_;
	monitor::texture pressure_;
	monitor::texture velocity_magnitude_;
	monitor::texture divergence_;
	simulation::base &simulation_;
};
}
}

#endif
