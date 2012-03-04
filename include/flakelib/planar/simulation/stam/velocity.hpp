#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_VELOCITY_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_VELOCITY_HPP_INCLUDED

#include <flakelib/planar/float2_view.hpp>
#include <fcppt/strong_typedef.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::planar::float2_view,
	velocity);
}
}
}
}

#endif
