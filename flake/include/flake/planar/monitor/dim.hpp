#ifndef FLAKE_PLANAR_MONITOR_DIM_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_DIM_HPP_INCLUDED

#include <flake/planar/monitor/scalar.hpp>
#include <fcppt/math/dim/static.hpp>

namespace flake
{
namespace planar
{
namespace monitor
{
typedef
fcppt::math::dim::static_<monitor::scalar,2>
dim;
}
}
}

#endif
