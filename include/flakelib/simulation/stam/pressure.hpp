#ifndef FLAKELIB_SIMULATION_STAM_PRESSURE_HPP_INCLUDED
#define FLAKELIB_SIMULATION_STAM_PRESSURE_HPP_INCLUDED

#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace simulation
{
namespace stam
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::image::planar &,
	pressure);
}
}
}

#endif
