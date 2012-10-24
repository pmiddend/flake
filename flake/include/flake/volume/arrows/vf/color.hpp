#ifndef FLAKE_VOLUME_ARROWS_VF_COLOR_HPP_INCLUDED
#define FLAKE_VOLUME_ARROWS_VF_COLOR_HPP_INCLUDED

#include <flake/volume/arrows/vf/color_format.hpp>
#include <sge/renderer/vf/color.hpp>

namespace flake
{
namespace volume
{
namespace arrows
{
namespace vf
{
typedef
sge::renderer::vf::color
<
	flake::volume::arrows::vf::color_format
>
color;
}
}
}
}

#endif

