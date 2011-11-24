#ifndef FLAKELIB_LAPLACE_SOLVER_JACOBI_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_JACOBI_HPP_INCLUDED

#include <flakelib/laplace_solver/base.hpp>
#include <flakelib/laplace_solver/grid_scale.hpp>
#include <flakelib/laplace_solver/iterations.hpp>
#include <flakelib/planar_pool/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace laplace_solver
{
class jacobi
:
	public laplace_solver::base
{
FCPPT_NONCOPYABLE(
	jacobi);
public:
	explicit
	jacobi(
		flakelib::planar_pool::object &,
		sge::opencl::command_queue::object &,
		laplace_solver::grid_scale const &,
		laplace_solver::iterations const &);

	void
	solve(
		laplace_solver::rhs const &,
		laplace_solver::destination const &,
		laplace_solver::initial_guess const &,
		laplace_solver::boundary const &);

	flakelib::additional_planar_data const &
	additional_planar_data() const;

	~jacobi();
private:
	flakelib::planar_pool::object &planar_cache_;
	sge::opencl::command_queue::object &command_queue_;
	grid_scale::value_type const grid_scale_;
	iterations::value_type const iterations_;
	sge::opencl::program::object jacobi_program_;
	sge::opencl::kernel::object jacobi_kernel_;
	flakelib::additional_planar_data additional_planar_data_;
};
}
}

#endif
