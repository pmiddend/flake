#ifndef FLAKELIB_VISUALIZATION_MONITOR_POSITION_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_POSITION_HPP_INCLUDED

#include <sge/renderer/vector2.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::vector2,
	position);
}
}
}

#endif
