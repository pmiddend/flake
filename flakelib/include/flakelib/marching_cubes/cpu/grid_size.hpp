#ifndef FLAKELIB_MARCHING_CUBES_CPU_GRID_SIZE_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_CPU_GRID_SIZE_HPP_INCLUDED

#include <flakelib/marching_cubes/cpu/dim3.hpp>
#include <fcppt/strong_typedef.hpp>


namespace flakelib
{
namespace marching_cubes
{
namespace cpu
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::marching_cubes::cpu::dim3,
	grid_size);
}
}
}

#endif
