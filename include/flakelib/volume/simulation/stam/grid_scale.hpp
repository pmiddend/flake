#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_GRID_SCALE_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_GRID_SCALE_HPP_INCLUDED

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
	grid_scale);
}
}
}
}

#endif
