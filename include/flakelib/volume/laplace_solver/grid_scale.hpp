#ifndef FLAKELIB_VOLUME_LAPLACE_SOLVER_GRID_SCALE_HPP_INCLUDED
#define FLAKELIB_VOLUME_LAPLACE_SOLVER_GRID_SCALE_HPP_INCLUDED

#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace laplace_solver
{
FCPPT_MAKE_STRONG_TYPEDEF(
	cl_float,
	grid_scale);
}
}
}

#endif
