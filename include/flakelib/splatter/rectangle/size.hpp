#ifndef FLAKELIB_SPLATTER_RECTANGLE_SIZE_HPP_INCLUDED
#define FLAKELIB_SPLATTER_RECTANGLE_SIZE_HPP_INCLUDED

#include <sge/opencl/dim2.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace splatter
{
namespace rectangle
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::dim2,
	size);
}
}
}

#endif

