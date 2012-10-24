#ifndef FLAKELIB_MARCHING_CUBES_VF_POSITION_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_VF_POSITION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/pos.hpp>

namespace flakelib
{
namespace marching_cubes
{
namespace vf
{
typedef
sge::renderer::vf::pos
<
	sge::renderer::scalar,
	4
>
position;
}
}
}

#endif

