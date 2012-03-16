#ifndef FLAKE_PLANAR_MONITOR_VECTOR_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_VECTOR_HPP_INCLUDED

#include <flake/planar/monitor/scalar.hpp>
#include <fcppt/math/vector/static.hpp>

namespace flake
{
namespace planar
{
namespace monitor
{
typedef
fcppt::math::vector::static_<monitor::scalar,2>::type
vector;
}
}
}

#endif
