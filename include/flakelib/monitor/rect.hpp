#ifndef FLAKELIB_MONITOR_RECT_HPP_INCLUDED
#define FLAKELIB_MONITOR_RECT_HPP_INCLUDED

#include <flakelib/monitor/scalar.hpp>
#include <fcppt/math/box/rect.hpp>

namespace flakelib
{
namespace monitor
{
typedef
fcppt::math::box::rect<monitor::scalar>::type
rect;
}
}

#endif
