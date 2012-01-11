#ifndef FLAKELIB_VOLUME_LAPLACE_SOLVER_UNIQUE_BASE_PTR_HPP_INCLUDED
#define FLAKELIB_VOLUME_LAPLACE_SOLVER_UNIQUE_BASE_PTR_HPP_INCLUDED

#include <flakelib/volume/laplace_solver/base_fwd.hpp>
#include <fcppt/unique_ptr.hpp>

namespace flakelib
{
namespace volume
{
namespace laplace_solver
{
typedef
fcppt::unique_ptr<laplace_solver::base>
unique_base_ptr;
}
}
}

#endif
