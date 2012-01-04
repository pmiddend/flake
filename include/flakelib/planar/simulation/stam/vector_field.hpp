#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_VECTOR_FIELD_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_VECTOR_FIELD_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
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
	flakelib::buffer::planar_view<cl_float2> const &,
	vector_field);
}
}
}
}

#endif
