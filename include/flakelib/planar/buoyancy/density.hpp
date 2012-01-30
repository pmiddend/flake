#ifndef FLAKELIB_PLANAR_BUOYANCY_DENSITY_HPP_INCLUDED
#define FLAKELIB_PLANAR_BUOYANCY_DENSITY_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace planar
{
namespace buoyancy
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::planar_view<cl_float>,
	density);
}
}
}

#endif

