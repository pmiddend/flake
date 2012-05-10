#ifndef FLAKELIB_CL_GLOBAL_DIM3_HPP_INCLUDED
#define FLAKELIB_CL_GLOBAL_DIM3_HPP_INCLUDED

#include <sge/opencl/memory_object/dim3.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace cl
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::dim3,
	global_dim3);
}
}

#endif

