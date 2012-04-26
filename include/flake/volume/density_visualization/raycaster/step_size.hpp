#ifndef FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_STEP_SIZE_HPP_INCLUDED
#define FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_STEP_SIZE_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flake
{
namespace volume
{
namespace density_visualization
{
namespace raycaster
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::scalar,
	step_size);
}
}
}
}

#endif

