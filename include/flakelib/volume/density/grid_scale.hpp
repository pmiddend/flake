#ifndef FLAKELIB_VOLUME_DENSITY_GRID_SCALE_HPP_INCLUDED
#define FLAKELIB_VOLUME_DENSITY_GRID_SCALE_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace volume
{
namespace density
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	grid_scale);
}
}
}

#endif
