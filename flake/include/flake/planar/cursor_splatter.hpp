#ifndef FLAKE_PLANAR_CURSOR_SPLATTER_HPP_INCLUDED
#define FLAKE_PLANAR_CURSOR_SPLATTER_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
#include <flake/planar/monitor/texture_fwd.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/planar/float_view.hpp>
#include <flakelib/splatter/object_fwd.hpp>
#include <flakelib/splatter/pen/object_impl.hpp>
#include <flakelib/splatter/pen/planar.hpp>
#include <flakelib/splatter/rectangle/object.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/input/cursor/button_event_fwd.hpp>
#include <sge/input/cursor/move_event_fwd.hpp>
#include <sge/input/cursor/object_fwd.hpp>
#include <sge/input/cursor/optional_position.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/renderer/device/core_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/optional/object_impl.hpp>
#include <fcppt/signal/auto_connection.hpp>


namespace flake
{
namespace planar
{
class cursor_splatter
{
FCPPT_NONCOPYABLE(
	cursor_splatter);
public:
	FLAKE_DETAIL_SYMBOL
	cursor_splatter(
		monitor::texture &,
		flakelib::splatter::object &,
		sge::renderer::device::core &,
		sge::camera::base const &,
		sge::input::cursor::object &,
		cl_float,
		flakelib::splatter::pen::planar const &);

	FLAKE_DETAIL_SYMBOL
	void
	left_mouse_target(
		flakelib::planar::float_view const &);

	FLAKE_DETAIL_SYMBOL
	void
	right_mouse_target(
		flakelib::planar::float_view const &);

	FLAKE_DETAIL_SYMBOL
	void
	update(
		flakelib::duration const &);

	FLAKE_DETAIL_SYMBOL
	~cursor_splatter();
private:
	monitor::texture &monitor_texture_;
	flakelib::splatter::object &splatter_;
	sge::renderer::device::core &renderer_;
	sge::camera::base const &camera_;
	fcppt::optional::object<flakelib::planar::float_view> optional_left_mouse_target_;
	fcppt::optional::object<flakelib::planar::float_view> optional_right_mouse_target_;
	cl_float const splat_value_;
	flakelib::splatter::pen::planar const pen_;
	fcppt::signal::auto_connection button_connection_;
	fcppt::signal::auto_connection move_connection_;
	bool left_button_pushed_down_,right_button_pushed_down_;
	sge::input::cursor::optional_position last_cursor_position_;

	void
	button_callback(
		sge::input::cursor::button_event const &);

	void
	move_callback(
		sge::input::cursor::move_event const &);

	void
	splat_at_cursor_position(
		flakelib::planar::float_view const &,
		sge::input::cursor::position const &,
		flakelib::duration const &);

	sge::renderer::vector2 const
	unproject_cursor_position(
		sge::input::cursor::position const &);
};
}
}

#endif

