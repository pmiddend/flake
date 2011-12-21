#ifndef FLAKELIB_UTILITY_LINEAR_FLOAT_SOURCE_HPP_INCLUDED
#define FLAKELIB_UTILITY_LINEAR_FLOAT_SOURCE_HPP_INCLUDED

#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace utility
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::buffer &,
	linear_float_source);
}
}

#endif
