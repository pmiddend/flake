#ifndef FLAKELIB_VOLUME_BOUNDARY_SPHERE_POSITION_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_SPHERE_POSITION_HPP_INCLUDED

#include <sge/opencl/memory_object/dim3.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace boundary
{
namespace sphere
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::dim3,
	position);
}
}
}
}

#endif
