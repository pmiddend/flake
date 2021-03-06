#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_RHS_BUFFER_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_RHS_BUFFER_VIEW_HPP_INCLUDED

#include <flakelib/buffer/volume_view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace simulation
{
namespace stam
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::volume_view<cl_float>,
	rhs_buffer_view);
}
}
}
}

#endif

