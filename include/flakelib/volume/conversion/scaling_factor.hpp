#ifndef FLAKELIB_VOLUME_CONVERSION_SCALING_FACTOR_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_SCALING_FACTOR_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace conversion
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	scaling_factor);
}
}
}

#endif

