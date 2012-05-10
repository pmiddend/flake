#ifndef FLAKELIB_CL_LOCAL_DIM2_HPP_INCLUDED
#define FLAKELIB_CL_LOCAL_DIM2_HPP_INCLUDED

#include <sge/opencl/memory_object/dim2.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace cl
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::dim2,
	local_dim2);
}
}

#endif

