#ifndef FLAKE_VOLUME_MODEL_FOG_DENSITY_HPP_INCLUDED
#define FLAKE_VOLUME_MODEL_FOG_DENSITY_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <fcppt/strong_typedef.hpp>


namespace flake
{
namespace volume
{
namespace model
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::scalar,
	fog_density);
}
}
}

#endif
