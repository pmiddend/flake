#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_PRESSURE_BUFFER_VIEW_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_PRESSURE_BUFFER_VIEW_HPP_INCLUDED

#include <flakelib/buffer/planar_view.hpp>
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
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::planar_view<cl_float>,
	pressure_buffer_view);
}
}
}
}

#endif

