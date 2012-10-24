#ifndef FLAKE_PLANAR_MONITOR_BORDER_SIZE_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_BORDER_SIZE_HPP_INCLUDED

#include <flake/planar/monitor/rect.hpp>
#include <fcppt/make_strong_typedef.hpp>
#include <fcppt/math/box/object_impl.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
FCPPT_MAKE_STRONG_TYPEDEF(
	monitor::rect::value_type,
	border_size);
}
}
}

#endif
