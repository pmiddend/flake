#ifndef FLAKELIB_VALUE_MODULATOR_FREQUENCY_HPP_INCLUDED
#define FLAKELIB_VALUE_MODULATOR_FREQUENCY_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace value_modulator
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	frequency);
}
}

#endif

