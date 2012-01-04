#ifndef FLAKELIB_PLANAR_MONITOR_GRID_DIMENSIONS_HPP_INCLUDED
#define FLAKELIB_PLANAR_MONITOR_GRID_DIMENSIONS_HPP_INCLUDED

#include <sge/renderer/dim2.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace planar
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
