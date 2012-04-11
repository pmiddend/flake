#ifndef FLAKE_VOLUME_MODEL_POSITION_HPP_INCLUDED
#define FLAKE_VOLUME_MODEL_POSITION_HPP_INCLUDED

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
	position);
}
}
}

#endif

