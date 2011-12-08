#ifndef FLAKELIB_DENSITY_VELOCITY_IMAGE_HPP_INCLUDED
#define FLAKELIB_DENSITY_VELOCITY_IMAGE_HPP_INCLUDED

#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace density
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::image::planar &,
	velocity_image);
}
}

#endif
