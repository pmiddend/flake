#ifndef FLAKELIB_LAPLACE_SOLVER_BASE_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_BASE_HPP_INCLUDED

#include <flakelib/laplace_solver/destination.hpp>
#include <flakelib/laplace_solver/rhs.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
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
		laplace_solver::rhs const &,
		laplace_solver::destination const &) = 0;

	virtual
	~base() = 0;
protected:
	explicit
	base();
};
}
}

#endif
