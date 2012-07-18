#ifndef FLAKELIB_MARCHING_CUBES_CPU_GRID_SIZE_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_CPU_GRID_SIZE_HPP_INCLUDED

#include <sge/renderer/dim3.hpp>
#include <fcppt/strong_typedef.hpp>


namespace flakelib
{
namespace marching_cubes
{
namespace cpu
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::dim3,
	grid_size);
}
}
}

#endif
