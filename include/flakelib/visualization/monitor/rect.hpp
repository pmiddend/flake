#ifndef FLAKELIB_VISUALIZATION_MONITOR_RECT_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_RECT_HPP_INCLUDED

#include <flakelib/visualization/monitor/scalar.hpp>
#include <fcppt/math/box/rect.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
typedef
fcppt::math::box::rect<monitor::scalar>::type
rect;
}
}
}

#endif
