#ifndef FLAKELIB_PLANAR_BUOYANCY_DENSITY_STRENGTH_HPP_INCLUDED
#define FLAKELIB_PLANAR_BUOYANCY_DENSITY_STRENGTH_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace planar
{
namespace buoyancy
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	density_strength);
}
}
}

#endif

