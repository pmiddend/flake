#ifndef FLAKELIB_VISUALIZATION_MONITOR_GRID_SCALE_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_GRID_SCALE_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::scalar,
	grid_scale);
}
}
}

#endif
