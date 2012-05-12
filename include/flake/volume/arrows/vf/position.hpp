#ifndef FLAKE_VOLUME_ARROWS_VF_POSITION_HPP_INCLUDED
#define FLAKE_VOLUME_ARROWS_VF_POSITION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/pos.hpp>

namespace flake
{
namespace volume
{
namespace arrows
{
namespace vf
{
typedef
sge::renderer::vf::pos
<
	sge::renderer::scalar,
	4
>
position;
}
}
}
}

#endif

