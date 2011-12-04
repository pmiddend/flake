#ifndef FLAKELIB_DENSITY_SPLAT_RADIUS_HPP_INCLUDED
#define FLAKELIB_DENSITY_SPLAT_RADIUS_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/opencl/memory_object/size_type.hpp>

namespace flakelib
{
namespace density
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::size_type,
	splat_radius);
}
}

#endif
