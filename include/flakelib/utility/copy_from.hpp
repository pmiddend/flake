#ifndef FLAKELIB_UTILITY_COPY_FROM_HPP_INCLUDED
#define FLAKELIB_UTILITY_COPY_FROM_HPP_INCLUDED

#include <flakelib/buffer/linear_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace utility
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::linear_view<cl_float> const &,
	copy_from);
}
}

#endif
