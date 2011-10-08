#ifndef FLAKELIB_VISUALIZATION_MONITOR_GRID_DIMENSIONS_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_GRID_DIMENSIONS_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/renderer/dim2.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::dim2,
	grid_dimensions);
}
}
}

#endif
