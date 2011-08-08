#ifndef FLAKE_SCALAR_COLOR_FORMAT_HPP_INCLUDED
#define FLAKE_SCALAR_COLOR_FORMAT_HPP_INCLUDED

#include "scalar.hpp"
#include <mizuiro/color/homogenous_static.hpp>
#include <mizuiro/image/format.hpp>
#include <mizuiro/image/dimension.hpp>
#include <mizuiro/image/interleaved.hpp>
#include <mizuiro/color/layout/luminance.hpp>

namespace flake
{
typedef 
mizuiro::image::format<
	mizuiro::image::dimension<2>,
	mizuiro::image::interleaved
	<
		mizuiro::color::homogenous_static
		<
			flake::scalar,
			mizuiro::color::layout::luminance
		>
	>
> 
scalar_color_format;
}

#endif
