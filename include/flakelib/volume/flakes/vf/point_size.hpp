#ifndef FLAKELIB_VOLUME_FLAKES_VF_POINT_SIZE_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_VF_POINT_SIZE_HPP_INCLUDED

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
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(point_size);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		1
	>,
	tags::point_size
>
point_size;
}
}
}
}

#endif
