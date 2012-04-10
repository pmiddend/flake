#ifndef FLAKELIB_VOLUME_GRID_SIZE_HPP_INCLUDED
#define FLAKELIB_VOLUME_GRID_SIZE_HPP_INCLUDED

#include <sge/opencl/memory_object/dim3.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::dim3,
	grid_size);
}
}

#endif
