#ifndef FLAKELIB_VISUALIZATION_ARROW_VF_COLOR_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_ARROW_VF_COLOR_HPP_INCLUDED

#include <sge/renderer/vf/unspecified.hpp>
#include <sge/renderer/vf/vector.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/make_unspecified_tag.hpp>

namespace flakelib
{
namespace visualization
{
namespace arrow_vf
{
namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(
	color);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector<sge::renderer::scalar,4>,
	tags::color
>
color;
}
}
}

#endif
