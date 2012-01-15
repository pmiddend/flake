#ifndef FLAKELIB_VOLUME_FLAKES_VF_STARTING_POSITION_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_VF_STARTING_POSITION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/make_unspecified_tag.hpp>
#include <sge/renderer/vf/unspecified.hpp>
#include <sge/renderer/vf/vector.hpp>


namespace flakelib
{
namespace volume
{
namespace flakes
{
namespace vf
{
namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(starting_position);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		4
	>,
	tags::starting_position
>
starting_position;
}
}
}
}

#endif
