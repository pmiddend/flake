#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_GRAVITY_MAGNITUDE_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_GRAVITY_MAGNITUDE_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
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
	cl_float,
	gravity_magnitude);
}
}
}
}

#endif
