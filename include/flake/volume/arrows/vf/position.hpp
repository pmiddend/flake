#ifndef FLAKE_VOLUME_ARROWS_VF_POSITION_HPP_INCLUDED
#define FLAKE_VOLUME_ARROWS_VF_POSITION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/make_unspecified_tag.hpp>
#include <sge/renderer/vf/unspecified.hpp>
#include <sge/renderer/vf/vector.hpp>

namespace flake
{
namespace volume
{
namespace arrows
{
namespace vf
{
namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(
	position);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		4
	>,
	tags::position
>
position;
}
}
}
}

#endif

