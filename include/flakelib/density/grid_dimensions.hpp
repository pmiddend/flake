#ifndef FLAKELIB_DENSITY_GRID_DIMENSIONS_HPP_INCLUDED
#define FLAKELIB_DENSITY_GRID_DIMENSIONS_HPP_INCLUDED

#include <sge/opencl/memory_object/dim2.hpp>

namespace flakelib
{
namespace density
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::dim2,
	grid_dimensions);
}
}

#endif
