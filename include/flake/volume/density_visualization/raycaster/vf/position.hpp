#ifndef FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_VF_POSITION_HPP_INCLUDED
#define FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_VF_POSITION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/pos.hpp>

namespace flake
{
namespace volume
{
namespace density_visualization
{
namespace raycaster
{
namespace vf
{
typedef
sge::renderer::vf::pos
<
	sge::renderer::scalar,
	3
>
position;
}
}
}
}
}

#endif

