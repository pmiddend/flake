#ifndef FLAKELIB_SPLATTER_PEN_BLEND_FACTOR_HPP_INCLUDED
#define FLAKELIB_SPLATTER_PEN_BLEND_FACTOR_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace splatter
{
namespace pen
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	blend_factor);
}
}
}

#endif

