#ifndef FLAKELIB_SPLATTER_CIRCLE_POSITION_HPP_INCLUDED
#define FLAKELIB_SPLATTER_CIRCLE_POSITION_HPP_INCLUDED

#include <sge/opencl/memory_object/dim2.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace splatter
{
namespace circle
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::dim2,
	position);
}
}
}

#endif

