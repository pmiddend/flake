#ifndef FLAKELIB_PLANAR_DENSITY_CURSOR_RECTANGLE_HPP_INCLUDED
#define FLAKELIB_PLANAR_DENSITY_CURSOR_RECTANGLE_HPP_INCLUDED

#include <sge/input/cursor/position_unit.hpp>
#include <fcppt/math/box/rect.hpp>

namespace flakelib
{
namespace planar
{
namespace density
{
typedef
fcppt::math::box::rect<sge::input::cursor::position_unit>::type
cursor_rectangle;
}
}
}

#endif
