#ifndef FLAKELIB_VOLUME_FLAKES_PARTICLE_MINIMUM_SIZE_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_PARTICLE_MINIMUM_SIZE_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace flakes
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	particle_minimum_size);
}
}
}

#endif
