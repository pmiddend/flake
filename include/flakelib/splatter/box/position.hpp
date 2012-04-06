#ifndef FLAKELIB_SPLATTER_BOX_POSITION_HPP_INCLUDED
#define FLAKELIB_SPLATTER_BOX_POSITION_HPP_INCLUDED

#include <sge/opencl/memory_object/vector3.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace splatter
{
namespace box
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::vector3,
	position);
}
}
}

#endif

