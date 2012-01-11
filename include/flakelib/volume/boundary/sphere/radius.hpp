#ifndef FLAKELIB_VOLUME_BOUNDARY_SPHERE_RADIUS_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_SPHERE_RADIUS_HPP_INCLUDED

#include <sge/opencl/memory_object/size_type.hpp>
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
	sge::opencl::memory_object::size_type,
	radius);
}
}
}
}

#endif
