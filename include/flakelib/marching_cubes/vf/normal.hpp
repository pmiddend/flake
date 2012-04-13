#ifndef FLAKELIB_MARCHING_CUBES_VF_NORMAL_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_VF_NORMAL_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/make_unspecified_tag.hpp>
#include <sge/renderer/vf/unspecified.hpp>
#include <sge/renderer/vf/vector.hpp>

namespace flakelib
{
namespace marching_cubes
{
namespace vf
{
namespace tags
{
SGE_RENDERER_VF_MAKE_UNSPECIFIED_TAG(
	normal);
}

typedef
sge::renderer::vf::unspecified
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		4
	>,
	tags::normal
>
normal;
}
}
}

#endif

