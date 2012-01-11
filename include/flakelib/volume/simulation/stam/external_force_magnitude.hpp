#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_EXTERNAL_FORCE_MAGNITUDE_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_EXTERNAL_FORCE_MAGNITUDE_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/opencl/clinclude.hpp>

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
	external_force_magnitude);
}
}
}
}

#endif
