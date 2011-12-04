#ifndef FLAKELIB_MONITOR_SCALING_FACTOR_HPP_INCLUDED
#define FLAKELIB_MONITOR_SCALING_FACTOR_HPP_INCLUDED

#include <flakelib/monitor/scalar.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace monitor
{
FCPPT_MAKE_STRONG_TYPEDEF(
	monitor::scalar,
	scaling_factor);
}
}

#endif
