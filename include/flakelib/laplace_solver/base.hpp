#ifndef FLAKELIB_LAPLACE_SOLVER_BASE_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_BASE_HPP_INCLUDED

#include <flakelib/additional_planar_data.hpp>
#include <flakelib/laplace_solver/destination.hpp>
#include <flakelib/laplace_solver/initial_guess.hpp>
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
		laplace_solver::destination const &,
		laplace_solver::initial_guess const &) = 0;

	virtual flakelib::additional_planar_data const &
	additional_planar_data() const = 0;

	virtual
	~base() = 0;
protected:
	explicit
	base();
};
}
}

#endif
