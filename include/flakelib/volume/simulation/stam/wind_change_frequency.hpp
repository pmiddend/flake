#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_WIND_CHANGE_FREQUENCY_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_WIND_CHANGE_FREQUENCY_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
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
	cl_float,
	wind_change_frequency);
}
}
}
}

#endif
