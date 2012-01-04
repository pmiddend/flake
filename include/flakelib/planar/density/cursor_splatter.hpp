#ifndef FLAKELIB_PLANAR_DENSITY_CURSOR_SPLATTER_HPP_INCLUDED
#define FLAKELIB_PLANAR_DENSITY_CURSOR_SPLATTER_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/planar/density/cursor_rectangle.hpp>
#include <flakelib/planar/density/source_image.hpp>
#include <flakelib/planar/density/splat_radius.hpp>
#include <sge/input/cursor/button_event_fwd.hpp>
#include <sge/input/cursor/move_event_fwd.hpp>
#include <sge/input/cursor/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/signal/scoped_connection.hpp>


namespace flakelib
{
namespace planar
{
namespace density
{
/**
\brief Modify source image using mouse clicks

This class takes a cursor, a rectangle to click inside and the source image.
When the mouse is clicked inside the rectangle, the cursor position is
translated from window coordinates to viewport coordinates. After that, the
position inside the source grid is determined and a splat is inserted at the
corresponding position.
*/
class cursor_splatter
{
FCPPT_NONCOPYABLE(
	cursor_splatter);
public:
	explicit
	cursor_splatter(
		sge::opencl::command_queue::object &,
		density::source_image const &,
		sge::input::cursor::object &,
		sge::renderer::device &,
		flakelib::build_options const &,
		density::splat_radius const &);

	void
	update_cursor_rectangle(
		density::cursor_rectangle const &);

	~cursor_splatter();
private:
	sge::opencl::command_queue::object &command_queue_;
	flakelib::buffer::planar_view<cl_float> source_image_;
	sge::input::cursor::object &cursor_;
	sge::renderer::device &renderer_;
	sge::opencl::memory_object::size_type const splat_radius_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object splat_kernel_;
	fcppt::signal::scoped_connection cursor_button_connection_;
	fcppt::signal::scoped_connection cursor_move_connection_;
	bool is_being_dragged_;
	density::cursor_rectangle cursor_rectangle_;

	void
	cursor_button_callback(
		sge::input::cursor::button_event const &);

	void
	cursor_move_callback(
		sge::input::cursor::move_event const &);

	void
	splat_at_cursor_position();
};
}
}
}

#endif
