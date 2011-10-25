#ifndef FLAKELIB_VISUALIZATION_MONITOR_DIM_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_DIM_HPP_INCLUDED

#include <flakelib/visualization/monitor/scalar.hpp>
#include <fcppt/math/dim/static.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
typedef
fcppt::math::dim::static_<monitor::scalar,2>::type
dim;
}
}
}

#endif
