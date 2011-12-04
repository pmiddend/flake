#ifndef FLAKELIB_MONITOR_VECTOR_HPP_INCLUDED
#define FLAKELIB_MONITOR_VECTOR_HPP_INCLUDED

#include <flakelib/monitor/scalar.hpp>
#include <fcppt/math/vector/static.hpp>

namespace flakelib
{
namespace monitor
{
typedef
fcppt::math::vector::static_<monitor::scalar,2>::type
vector;
}
}

#endif
