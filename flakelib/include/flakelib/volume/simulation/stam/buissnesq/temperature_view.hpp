#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_BUISSNESQ_TEMPERATURE_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_BUISSNESQ_TEMPERATURE_VIEW_HPP_INCLUDED

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
namespace buissnesq
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::volume::float_view,
	temperature_view);
}
}
}
}
}

#endif

