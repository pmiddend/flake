#include <flake/planar/cursor_splatter.hpp>
#include <flake/planar/monitor/texture.hpp>
#include <flakelib/splatter/object.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/input/cursor/button_callback.hpp>
#include <sge/input/cursor/button_event.hpp>
#include <sge/input/cursor/move_callback.hpp>
#include <sge/input/cursor/move_event.hpp>
#include <sge/input/cursor/object.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/renderer/device/core.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <fcppt/cast/float_to_int_fun.hpp>
#include <fcppt/cast/int_to_float_fun.hpp>
#include <fcppt/cast/size_fun.hpp>
#include <fcppt/math/size_type.hpp>
#include <fcppt/math/box/contains_point.hpp>
#include <fcppt/math/box/rect.hpp>
#include <fcppt/math/box/structure_cast.hpp>
#include <fcppt/math/dim/to_vector.hpp>
#include <fcppt/math/matrix/has_dim.hpp>
#include <fcppt/math/matrix/inverse.hpp>
#include <fcppt/math/matrix/object_impl.hpp>
#include <fcppt/math/matrix/vector.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/has_dim.hpp>
#include <fcppt/math/vector/narrow_cast.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/math/vector/static.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/math/vector/to_signed.hpp>
#include <fcppt/math/vector/to_unsigned.hpp>
#include <fcppt/optional/maybe_void.hpp>
#include <fcppt/signal/connection.hpp>
#include <fcppt/config/external_begin.hpp>
#include <functional>
#include <type_traits>
#include <fcppt/config/external_end.hpp>


namespace
{
/**
\brief Does roughly the same as gluUnproject
\tparam T The matrix's and vector's value type
\tparam S1 The vector's storage type
\tparam S2 The matrix's storage type
\param viewport_coordinates Three-dimensional viewport-relative coordinates. You can pass a depth which specifies the distance from the viewing plane (the unprojected point is, of course, not unique)
\param inverse_mvp The already-inverted model-view-projection matrix. This is a performance improvement over gluUnproject since you might only calculate this matrix once and then reuse it
\param viewport The viewport

For a description of gluUnproject, see:

http://www.opengl.org/sdk/docs/man/xhtml/gluUnProject.xml

The problem with gluUnproject is that it's tailor-made for a very specific
purpose: Unproject a <em>single</em> point given in <em>window
coordinates</em>. However, you often want to unproject more than one point.
This is extremely slow using gluUnproject because the MVP matrix has to be
inverted each time.

Also, you might already have coordinates in viewport coordinates, so the
translation is unnecessary.

\note
Due to technical difficulties, this function only works with static vectors. (I
need a function to retrieve a vector of dimension N+1).
*/
template<
	typename T,
	typename S1,
	typename S2
>
typename
boost::enable_if
<
	std::is_floating_point<T>,
	fcppt::math::vector::static_<T,3>
>::type
unproject(
	fcppt::math::vector::object<T,3,S1> const &viewport_coordinates,
	fcppt::math::matrix::object<T,4,4,S2> const &inverse_mvp,
	fcppt::math::box::object<T,2> const &viewport)
{
	typedef
	fcppt::math::vector::static_
	<
		T,
		4
	>
	vector4;

	// Copied from
	// http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
	// and adapted to the ugly static_cast fcppt style
	//
	// This first transforms the window coordinates to values in [-1,1] (on
	// the x and y axis, respectively). Then we've got a point in the
	// canonical view volume (the volume all projection matrices project
	// into). Then the inverse mvp matrix is applied, resulting in a world
	// space coordinate. Then the w division is performed.
	vector4 result =
		inverse_mvp *
		vector4(
			static_cast<T>(
				static_cast<T>(2) * viewport_coordinates.x() / viewport.size().w() - static_cast<T>(1)),
			static_cast<T>(
				static_cast<T>(2) * viewport_coordinates.y() / viewport.size().h() - static_cast<T>(1)),
			static_cast<T>(
				static_cast<T>(2) * viewport_coordinates.z()),
			static_cast<T>(
				static_cast<T>(1)));

	result.w() = static_cast<T>(1)/result.w();

	return
		fcppt::math::vector::static_<T,3>(
			static_cast<T>(result.w() * result.x()),
			static_cast<T>(result.w() * result.y()),
			static_cast<T>(result.w() * result.z()));
}
}

flake::planar::cursor_splatter::cursor_splatter(
	monitor::texture &_monitor_texture,
	flakelib::splatter::object &_splatter,
	sge::renderer::device::core &_renderer,
	sge::camera::base const &_camera,
	sge::input::cursor::object &_cursor,
	cl_float const _splat_value,
	flakelib::splatter::pen::planar const &_pen)
:
	monitor_texture_(
		_monitor_texture),
	splatter_(
		_splatter),
	renderer_(
		_renderer),
	camera_(
		_camera),
	optional_left_mouse_target_(),
	optional_right_mouse_target_(),
	splat_value_(
		_splat_value),
	pen_(
		_pen),
	button_connection_(
		_cursor.button_callback(
			sge::input::cursor::button_callback{
				std::bind(
					&cursor_splatter::button_callback,
					this,
					std::placeholders::_1
				)
			}
		)
	),
	move_connection_(
		_cursor.move_callback(
			sge::input::cursor::move_callback{
				std::bind(
					&cursor_splatter::move_callback,
					this,
					std::placeholders::_1
				)
			}
		)
	),
	left_button_pushed_down_(
		false),
	right_button_pushed_down_(
		false),
	last_cursor_position_()
{
}

void
flake::planar::cursor_splatter::left_mouse_target(
	flakelib::planar::float_view const &_f)
{
	optional_left_mouse_target_ =
		fcppt::optional::object<flakelib::planar::float_view>(
			_f);
}

void
flake::planar::cursor_splatter::right_mouse_target(
	flakelib::planar::float_view const &_f)
{
	optional_right_mouse_target_ =
		fcppt::optional::object<flakelib::planar::float_view>(
			_f);
}

void
flake::planar::cursor_splatter::update(
	flakelib::duration const &_delta)
{
	fcppt::optional::maybe_void(
		last_cursor_position_,
		[
			this,
			_delta
		](
			sge::input::cursor::position const _last_cursor_position
		)
		{
			fcppt::optional::maybe_void(
				optional_left_mouse_target_,
				[
					this,
					_delta,
					_last_cursor_position
				](
					flakelib::planar::float_view const &_left_mouse_target
				)
				{
					if(left_button_pushed_down_)
						this->splat_at_cursor_position(
							_left_mouse_target,
							_last_cursor_position,
							_delta);
				}
			);

			fcppt::optional::maybe_void(
				optional_right_mouse_target_,
				[
					this,
					_delta,
					_last_cursor_position
				](
					flakelib::planar::float_view const &_right_mouse_target
				)
				{

					if(right_button_pushed_down_)
						this->splat_at_cursor_position(
							_right_mouse_target,
							_last_cursor_position,
							_delta);
				}
			);
		}
	);
}

flake::planar::cursor_splatter::~cursor_splatter()
{
}

void
flake::planar::cursor_splatter::button_callback(
	sge::input::cursor::button_event const &e)
{
	if(e.button_code() == sge::input::cursor::button_code::left)
	{
		left_button_pushed_down_ =
			e.pressed();
	}

	if(e.button_code() == sge::input::cursor::button_code::right)
	{
		right_button_pushed_down_ =
			e.pressed();
	}
}

void
flake::planar::cursor_splatter::move_callback(
	sge::input::cursor::move_event const &e)
{
	last_cursor_position_ =
		e.position();
}

void
flake::planar::cursor_splatter::splat_at_cursor_position(
	flakelib::planar::float_view const &target,
	sge::input::cursor::position const &cursor_position_window_coords,
	flakelib::duration const &_delta)
{
	sge::renderer::vector2 const unprojected_lefttop(
		this->unproject_cursor_position(
			cursor_position_window_coords));

	typedef
	fcppt::math::box::rect<sge::renderer::scalar>
	renderer_scalar_rect;

	renderer_scalar_rect const
		// This is the monitor rectangle with the origin at the left
		// top (relative to the viewport).
		monitor_rectangle_origin_lefttop =
			fcppt::math::box::structure_cast<
				renderer_scalar_rect,
				fcppt::cast::int_to_float_fun>(
				monitor_texture_.content_widget().area());

	// The monitor rectangle is also absolute, so this works
	if(!fcppt::math::box::contains_point(monitor_rectangle_origin_lefttop,unprojected_lefttop))
		return;

	typedef
	flakelib::splatter::rectangle::size::value_type
	discrete_dim2;

	typedef
	discrete_dim2::value_type
	discrete_unit;

	typedef
	flakelib::splatter::rectangle::position::value_type
	discrete_vector2;

	typedef
	decltype(
		fcppt::math::vector::to_signed(
			std::declval<
				flakelib::splatter::rectangle::position::value_type
			>()
		)
	)
	signed_discrete_vector2;

	sge::renderer::vector2 const
		unprojected_relative_lefttop(
			unprojected_lefttop - monitor_rectangle_origin_lefttop.pos());

	// Make it a discrete position
	flakelib::splatter::rectangle::object new_pen_rectangle(
		flakelib::splatter::rectangle::position(
			pen_.bounding().position().get() +
			fcppt::math::vector::to_unsigned(
				fcppt::math::vector::structure_cast<
					signed_discrete_vector2,
					fcppt::cast::float_to_int_fun>(
						unprojected_relative_lefttop)) -
			fcppt::math::vector::structure_cast<
				discrete_vector2,
				fcppt::cast::size_fun>(
				fcppt::math::dim::to_vector(
					pen_.bounding().size().get() /
					static_cast<discrete_unit>(
						2u)))),
		pen_.bounding().size());

	splatter_.splat_planar_float(
		target,
		flakelib::splatter::pen::planar(
			pen_,
			new_pen_rectangle),
		_delta.count() * splat_value_);
}

sge::renderer::vector2 const
flake::planar::cursor_splatter::unproject_cursor_position(
	sge::input::cursor::position const &cursor_position_window_coords)
{
	sge::renderer::pixel_rect const current_viewport(
		renderer_.onscreen_target().viewport().get());

	// The cursor position is in _window coordinates_. The entities we're
	// dealing with (the monitor rectangle) are relative to the _viewport_,
	// however. If the viewport does not fill the whole window, we've got a
	// problem. So we're converting to viewport coordinates here.
	sge::input::cursor::position const
		cursor_position_viewport_coords(
			cursor_position_window_coords -
			fcppt::math::vector::structure_cast<
				sge::input::cursor::position,
				fcppt::cast::size_fun>(
				current_viewport.pos()));

	typedef
	fcppt::math::box::rect<sge::renderer::scalar>
	renderer_scalar_rect;

	renderer_scalar_rect const
		// We need the viewport to translate the origins from left top
		// to left bottom and backwards.
		current_floating_point_viewport =
			fcppt::math::box::structure_cast<
				renderer_scalar_rect,
				fcppt::cast::int_to_float_fun>(
				current_viewport),
		// This is the monitor rectangle with the origin at the left
		// top (relative to the viewport).
		monitor_rectangle_origin_lefttop =
			fcppt::math::box::structure_cast<
				renderer_scalar_rect,
				fcppt::cast::int_to_float_fun>(
				monitor_texture_.content_widget().area());

	sge::renderer::vector3 const
		floating_point_cursor_position_leftbottom(
			static_cast<sge::renderer::scalar>(
				cursor_position_viewport_coords.x()),
			current_floating_point_viewport.size().h() -
			static_cast<sge::renderer::scalar>(
				cursor_position_viewport_coords.y()),
			static_cast<sge::renderer::scalar>(
				0.0f));

	// We pass in coordinates which are relative to "left bottom" and
	// receive a value which is relative to "left top".
	return
		fcppt::math::vector::narrow_cast<sge::renderer::vector2>(
				unproject(
					floating_point_cursor_position_leftbottom,
					fcppt::math::matrix::inverse(
						sge::camera::matrix_conversion::world_projection(
							camera_.coordinate_system(),
							camera_.projection_matrix())),
					current_floating_point_viewport));
}
