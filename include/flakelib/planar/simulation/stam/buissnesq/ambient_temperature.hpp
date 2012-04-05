#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_BUISSNESQ_AMBIENT_TEMPERATURE_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_BUISSNESQ_AMBIENT_TEMPERATURE_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
namespace buissnesq
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	ambient_temperature);
}
}
}
}
}

#endif

