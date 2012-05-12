#ifndef FLAKELIB_MARCHING_CUBES_VF_NORMAL_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_VF_NORMAL_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/extra.hpp>
#include <sge/renderer/vf/vector.hpp>
#include <sge/renderer/vf/index.hpp>

namespace flakelib
{
namespace marching_cubes
{
namespace vf
{
typedef
sge::renderer::vf::extra
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		4u
	>,
	sge::renderer::vf::index<0u>
>
normal;
}
}
}

#endif

