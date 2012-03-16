#ifndef FLAKE_PLANAR_CURSOR_SPLATTER_HPP_INCLUDED
#define FLAKE_PLANAR_CURSOR_SPLATTER_HPP_INCLUDED

#include <sge/camera/base_fwd.hpp>
#include <sge/input/cursor/object_fwd.hpp>
#include <sge/input/cursor/button_event_fwd.hpp>
#include <sge/input/cursor/move_event_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <flakelib/planar/float_view.hpp>
#include <flake/planar/monitor/texture_fwd.hpp>
#include <flakelib/splatter/object_fwd.hpp>
#include <flakelib/splatter/pen/planar.hpp>
#include <flakelib/splatter/pen/object_impl.hpp>
#include <flakelib/splatter/rectangle/object.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <fcppt/optional_impl.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/signal/scoped_connection.hpp>

namespace flake
{
namespace planar
{
class cursor_splatter
{
FCPPT_NONCOPYABLE(
	cursor_splatter);
public:
	explicit
	cursor_splatter(
		monitor::texture &,
		flakelib::splatter::object &,
		sge::renderer::device &,
		sge::camera::base &,
		sge::input::cursor::object &,
		cl_float,
		flakelib::splatter::pen::planar const &);

	void
	target(
		flakelib::planar::float_view const &);

	~cursor_splatter();
private:
	monitor::texture &monitor_texture_;
	flakelib::splatter::object &splatter_;
	sge::renderer::device &renderer_;
	sge::camera::base &camera_;
	fcppt::optional<flakelib::planar::float_view> optional_target_;
	cl_float const splat_value_;
	flakelib::splatter::pen::planar const pen_;
	fcppt::signal::scoped_connection button_connection_;
	fcppt::signal::scoped_connection move_connection_;
	bool button_pushed_down_;

	void
	button_callback(
		sge::input::cursor::button_event const &);

	void
	move_callback(
		sge::input::cursor::move_event const &);
};
}
}

#endif

