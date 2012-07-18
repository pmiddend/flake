#ifndef FLAKELIB_MARCHING_CUBES_GPU_POSITIONS_BUFFER_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_GPU_POSITIONS_BUFFER_HPP_INCLUDED

#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace marching_cubes
{
namespace gpu
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::buffer &,
	positions_buffer);
}
}
}

#endif

