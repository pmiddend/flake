#ifndef FLAKELIB_VOLUME_DENSITY_GRID_SIZE_HPP_INCLUDED
#define FLAKELIB_VOLUME_DENSITY_GRID_SIZE_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/opencl/memory_object/dim3.hpp>

namespace flakelib
{
namespace volume
{
namespace density
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::dim3,
	grid_size);
}
}
}

#endif
