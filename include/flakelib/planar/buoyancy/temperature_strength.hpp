#ifndef FLAKELIB_PLANAR_BUOYANCY_TEMPERATURE_STRENGTH_HPP_INCLUDED
#define FLAKELIB_PLANAR_BUOYANCY_TEMPERATURE_STRENGTH_HPP_INCLUDED

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
	temperature_strength);
}
}
}

#endif

