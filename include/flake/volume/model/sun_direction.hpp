#ifndef FLAKE_VOLUME_MODEL_SUN_DIRECTION_HPP_INCLUDED
#define FLAKE_VOLUME_MODEL_SUN_DIRECTION_HPP_INCLUDED

#include <sge/renderer/vector3.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flake
{
namespace volume
{
namespace model
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::vector3,
	sun_direction);
}
}
}

#endif

