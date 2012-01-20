#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_FORWARD_ADVECTED_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_FORWARD_ADVECTED_HPP_INCLUDED

#include <flakelib/buffer/planar_view.hpp>
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
	flakelib::buffer::planar_view<cl_float2>,
	forward_advected);
}
}
}
}

#endif
