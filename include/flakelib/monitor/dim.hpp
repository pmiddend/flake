#ifndef FLAKELIB_MONITOR_DIM_HPP_INCLUDED
#define FLAKELIB_MONITOR_DIM_HPP_INCLUDED

#include <flakelib/monitor/scalar.hpp>
#include <fcppt/math/dim/static.hpp>

namespace flakelib
{
namespace monitor
{
typedef
fcppt::math::dim::static_<monitor::scalar,2>::type
dim;
}
}

#endif
