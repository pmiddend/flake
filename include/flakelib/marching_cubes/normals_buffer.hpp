#ifndef FLAKELIB_MARCHING_CUBES_NORMALS_BUFFER_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_NORMALS_BUFFER_HPP_INCLUDED

#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace marching_cubes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::buffer &,
	normals_buffer);
}
}

#endif

