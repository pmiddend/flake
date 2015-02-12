#ifndef FLAKE_TEST_CAMERA_MANAGER_HPP_INCLUDED
#define FLAKE_TEST_CAMERA_MANAGER_HPP_INCLUDED

#include <sge/camera/base_fwd.hpp>
#include <sge/camera/perspective_projection_from_viewport.hpp>
#include <sge/camera/update_duration.hpp>
#include <sge/camera/tracking/json/interval_exporter_fwd.hpp>
#include <sge/input/keyboard/device_fwd.hpp>
#include <sge/input/keyboard/key_event_fwd.hpp>
#include <sge/input/mouse/device_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <memory>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace test
{
class camera_manager
{
FCPPT_NONCOPYABLE(
	camera_manager);
public:
	camera_manager(
		sge::parse::json::object const &,
		sge::input::keyboard::device &,
			sge::input::mouse::device &,
			sge::viewport::manager &);

	void
	update(
		sge::camera::update_duration const &);

	sge::camera::base &
	camera();

	~camera_manager();
private:
	std::unique_ptr<sge::camera::base> const camera_;
	sge::camera::perspective_projection_from_viewport perspective_projection_from_viewport_;
	std::unique_ptr<sge::camera::tracking::json::interval_exporter> const exporter_;
	fcppt::signal::scoped_connection const key_press_connection_;
	bool recording_;

	void
	key_press_callback(
		sge::input::keyboard::key_event const &);
};
}
}

#endif
