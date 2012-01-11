#ifndef FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_NORMAL_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_NORMAL_HPP_INCLUDED

#include <sge/renderer/vf/make_unspecified_tag.hpp>
#include <sge/renderer/vf/vector.hpp>
#include <sge/renderer/scalar.hpp>

namespace flakelib
{
namespace volume
{
namespace visualization
{
namespace model_vf
{
namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(normal);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		3
	>,
	tags::normal
>
normal;
}
}
}
}

#endif
