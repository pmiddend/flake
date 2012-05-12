#ifndef FLAKE_VOLUME_MODEL_VF_POSITION_HPP_INCLUDED
#define FLAKE_VOLUME_MODEL_VF_POSITION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/pos.hpp>

namespace flake
{
namespace volume
{
namespace model
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

#endif

