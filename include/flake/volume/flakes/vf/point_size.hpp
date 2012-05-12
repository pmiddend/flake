#ifndef FLAKE_VOLUME_FLAKES_VF_POINT_SIZE_HPP_INCLUDED
#define FLAKE_VOLUME_FLAKES_VF_POINT_SIZE_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <sge/renderer/vf/extra.hpp>
#include <sge/renderer/vf/index.hpp>
#include <sge/renderer/vf/vector.hpp>


namespace flake
{
namespace volume
{
namespace flakes
{
namespace vf
{
typedef
sge::renderer::vf::extra
<
	sge::renderer::vf::vector
	<
		sge::renderer::scalar,
		1u
	>,
	sge::renderer::vf::index<8u>
>
point_size;
}
}
}
}

#endif

