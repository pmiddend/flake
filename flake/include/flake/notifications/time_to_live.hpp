#ifndef FLAKE_NOTIFICATIONS_TIME_TO_LIVE_HPP_INCLUDED
#define FLAKE_NOTIFICATIONS_TIME_TO_LIVE_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flake
{
namespace notifications
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::duration,
	time_to_live);
}
}

#endif

