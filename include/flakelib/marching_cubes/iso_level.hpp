#ifndef FLAKELIB_MARCHING_CUBES_ISO_LEVEL_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_ISO_LEVEL_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace marching_cubes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	iso_level);
}
}

#endif

