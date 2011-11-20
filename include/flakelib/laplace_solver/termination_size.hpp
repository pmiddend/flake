#ifndef FLAKELIB_LAPLACE_SOLVER_TERMINATION_SIZE_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_TERMINATION_SIZE_HPP_INCLUDED

#include <sge/opencl/memory_object/size_type.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace laplace_solver
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::size_type,
	termination_size);
}
}

#endif
