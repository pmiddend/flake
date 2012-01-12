#ifndef FLAKELIB_VOLUME_CONVERSION_MULTIPLIER_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_MULTIPLIER_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace volume
{
namespace conversion
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	multiplier);
}
}
}

#endif
