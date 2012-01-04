#ifndef FLAKELIB_PLANAR_MONITOR_DIM_HPP_INCLUDED
#define FLAKELIB_PLANAR_MONITOR_DIM_HPP_INCLUDED

#include <flakelib/planar/monitor/scalar.hpp>
#include <fcppt/math/dim/static.hpp>

namespace flakelib
{
namespace planar
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
