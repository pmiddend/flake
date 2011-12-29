#ifndef FLAKELIB_DENSITY_SOURCE_IMAGE_HPP_INCLUDED
#define FLAKELIB_DENSITY_SOURCE_IMAGE_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace density
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::planar_view<cl_float>,
	source_image);
}
}

#endif
