#ifndef FLAKE_POSTPROCESSING_VF_POSITION_HPP_INCLUDED
#define FLAKE_POSTPROCESSING_VF_POSITION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/pos.hpp>

namespace flake
{
namespace postprocessing
{
namespace vf
{
typedef
sge::renderer::vf::pos
<
	sge::renderer::scalar,
	2
>
position;
}
}
}

#endif

