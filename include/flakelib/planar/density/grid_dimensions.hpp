#ifndef FLAKELIB_PLANAR_DENSITY_GRID_DIMENSIONS_HPP_INCLUDED
#define FLAKELIB_PLANAR_DENSITY_GRID_DIMENSIONS_HPP_INCLUDED

#include <sge/opencl/memory_object/dim2.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace planar
{
namespace density
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::dim2,
	grid_dimensions);
}
}
}

#endif
