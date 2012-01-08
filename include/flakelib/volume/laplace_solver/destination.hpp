#ifndef FLAKELIB_VOLUME_LAPLACE_SOLVER_DESTINATION_HPP_INCLUDED
#define FLAKELIB_VOLUME_LAPLACE_SOLVER_DESTINATION_HPP_INCLUDED

#include <flakelib/buffer/volume_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace laplace_solver
{
FCPPT_MAKE_STRONG_TYPEDEF(
	flakelib::buffer::volume_view<cl_float> const &,
	destination);
}
}
}

#endif
