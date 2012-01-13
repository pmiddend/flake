#ifndef FLAKELIB_VOLUME_DENSITY_CUBE_VF_POSITION_HPP_INCLUDED
#define FLAKELIB_VOLUME_DENSITY_CUBE_VF_POSITION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/make_unspecified_tag.hpp>
#include <sge/renderer/vf/unspecified.hpp>
#include <sge/renderer/vf/vector.hpp>


namespace flakelib
{
namespace volume
{
namespace density
{
namespace cube_vf
{
namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(position);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		3
	>,
	tags::position
>
position;
}
}
}
}

#endif
