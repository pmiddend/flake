#ifndef FLAKELIB_VOLUME_LAPLACE_SOLVER_BASE_HPP_INCLUDED
#define FLAKELIB_VOLUME_LAPLACE_SOLVER_BASE_HPP_INCLUDED

#include <flakelib/volume/laplace_solver/boundary.hpp>
#include <flakelib/volume/laplace_solver/destination.hpp>
#include <flakelib/volume/laplace_solver/initial_guess.hpp>
#include <flakelib/volume/laplace_solver/rhs.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace volume
{
namespace laplace_solver
{
class base
{
FCPPT_NONCOPYABLE(
	base);
public:
	virtual void
	solve(
		volume::laplace_solver::rhs const &,
		volume::laplace_solver::destination const &,
		volume::laplace_solver::initial_guess const &) = 0;

	virtual
	~base() = 0;
protected:
	explicit
	base();
};
}
}
}

#endif
