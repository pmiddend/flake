#ifndef FLAKELIB_PLANAR_DENSITY_GRID_SCALE_HPP_INCLUDED
#define FLAKELIB_PLANAR_DENSITY_GRID_SCALE_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace planar
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
