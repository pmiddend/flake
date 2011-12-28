#ifndef FLAKELIB_UTILITY_PLANAR_VECTOR_MAGNITUDE_FROM_HPP_INCLUDED
#define FLAKELIB_UTILITY_PLANAR_VECTOR_MAGNITUDE_FROM_HPP_INCLUDED

#include <flakelib/buffer/planar_view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace utility
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::planar_view<cl_float2>,
	planar_vector_magnitude_from);
}
}

#endif
