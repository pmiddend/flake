#ifndef FLAKELIB_PLANAR_LAPLACE_SOLVER_BASE_HPP_INCLUDED
#define FLAKELIB_PLANAR_LAPLACE_SOLVER_BASE_HPP_INCLUDED

#include <flakelib/planar/additional_scalar_data.hpp>
#include <flakelib/planar/laplace_solver/boundary.hpp>
#include <flakelib/planar/laplace_solver/destination.hpp>
#include <flakelib/planar/laplace_solver/initial_guess.hpp>
#include <flakelib/planar/laplace_solver/rhs.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
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
		planar::laplace_solver::rhs const &,
		planar::laplace_solver::destination const &,
		planar::laplace_solver::initial_guess const &,
		planar::laplace_solver::boundary const &) = 0;

	virtual planar::additional_scalar_data const &
	additional_scalar_data() const = 0;

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
