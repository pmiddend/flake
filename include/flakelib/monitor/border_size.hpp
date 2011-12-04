#ifndef FLAKELIB_MONITOR_BORDER_SIZE_HPP_INCLUDED
#define FLAKELIB_MONITOR_BORDER_SIZE_HPP_INCLUDED

#include <flakelib/monitor/rect.hpp>
#include <fcppt/make_strong_typedef.hpp>
#include <fcppt/math/box/basic_impl.hpp>


namespace flakelib
{
namespace monitor
{
FCPPT_MAKE_STRONG_TYPEDEF(
	monitor::rect::value_type,
	border_size);
}
}

#endif
