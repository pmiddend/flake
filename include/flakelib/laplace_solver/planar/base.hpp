#ifndef FLAKELIB_LAPLACE_SOLVER_PLANAR_BASE_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_PLANAR_BASE_HPP_INCLUDED

#include <flakelib/additional_planar_data.hpp>
#include <flakelib/laplace_solver/planar/boundary.hpp>
#include <flakelib/laplace_solver/planar/destination.hpp>
#include <flakelib/laplace_solver/planar/initial_guess.hpp>
#include <flakelib/laplace_solver/planar/rhs.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace laplace_solver
{
namespace planar
{
class base
{
FCPPT_NONCOPYABLE(
	base);
public:
	virtual void
	solve(
		laplace_solver::planar::rhs const &,
		laplace_solver::planar::destination const &,
		laplace_solver::planar::initial_guess const &,
		laplace_solver::planar::boundary const &) = 0;

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
}

#endif
