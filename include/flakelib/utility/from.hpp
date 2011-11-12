#ifndef FLAKELIB_UTILITY_FROM_HPP_INCLUDED
#define FLAKELIB_UTILITY_FROM_HPP_INCLUDED

#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace utility
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::image::planar &,
	from);
}
}

#endif
