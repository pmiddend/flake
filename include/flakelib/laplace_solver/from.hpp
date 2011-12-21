#ifndef FLAKELIB_LAPLACE_SOLVER_FROM_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_FROM_HPP_INCLUDED

#include <flakelib/buffer/planar_view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace laplace_solver
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::planar_view<cl_float> const &,
	from);
}
}

#endif
