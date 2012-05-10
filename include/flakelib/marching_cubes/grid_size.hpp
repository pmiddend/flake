#ifndef FLAKELIB_MARCHING_CUBES_GRID_SIZE_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_GRID_SIZE_HPP_INCLUDED

#include <flakelib/cl/uint4.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace marching_cubes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::cl::uint4,
	grid_size);
}
}

#endif

