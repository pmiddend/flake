#ifndef FLAKE_PLANAR_MONITOR_ARROW_VF_POSITION_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_ARROW_VF_POSITION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/pos.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
namespace arrow_vf
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
}

#endif
