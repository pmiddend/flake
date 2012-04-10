#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_PRESSURE_BUFFER_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_PRESSURE_BUFFER_VIEW_HPP_INCLUDED

#include <flakelib/volume/float_view.hpp>
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
	volume::float_view,
	pressure_buffer_view);
}
}
}
}

#endif

