#ifndef FLAKELIB_VOLUME_FLAKES_SNOW_TEXTURE_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_SNOW_TEXTURE_HPP_INCLUDED

#include <sge/image2d/view/const_object.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace flakes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::image2d::view::const_object,
	snow_texture);
}
}
}

#endif
