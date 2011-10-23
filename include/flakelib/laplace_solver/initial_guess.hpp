#ifndef FLAKELIB_LAPLACE_SOLVER_INITIAL_GUESS_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_INITIAL_GUESS_HPP_INCLUDED

#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace laplace_solver
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::image::planar &,
	initial_guess);
}
}

#endif
