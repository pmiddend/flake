#ifndef FLAKELIB_VOLUME_FLAKES_GRID_SIZE_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_GRID_SIZE_HPP_INCLUDED

#include <sge/opencl/memory_object/size_type.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace flakes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::size_type,
	grid_size);
}
}
}

#endif
