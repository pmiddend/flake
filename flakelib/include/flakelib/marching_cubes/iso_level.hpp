#ifndef FLAKELIB_MARCHING_CUBES_ISO_LEVEL_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_ISO_LEVEL_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>


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

