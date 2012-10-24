#ifndef FLAKELIB_SPLATTER_RECTANGLE_POSITION_HPP_INCLUDED
#define FLAKELIB_SPLATTER_RECTANGLE_POSITION_HPP_INCLUDED

#include <sge/opencl/vector2.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace splatter
{
namespace rectangle
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::vector2,
	position);
}
}
}

#endif

