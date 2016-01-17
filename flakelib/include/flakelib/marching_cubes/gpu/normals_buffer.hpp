#ifndef FLAKELIB_MARCHING_CUBES_GPU_NORMALS_BUFFER_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_GPU_NORMALS_BUFFER_HPP_INCLUDED

#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <fcppt/reference.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace marching_cubes
{
namespace gpu
{
FCPPT_MAKE_STRONG_TYPEDEF(
	fcppt::reference<
		sge::opencl::memory_object::buffer
	>,
	normals_buffer);
}
}
}

#endif

