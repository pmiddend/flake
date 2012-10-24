#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_VELOCITY_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_VELOCITY_HPP_INCLUDED

#include <flakelib/volume/float4_view.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace simulation
{
namespace stam
{
FCPPT_MAKE_STRONG_TYPEDEF(
	volume::float4_view,
	velocity);
}
}
}
}

#endif

