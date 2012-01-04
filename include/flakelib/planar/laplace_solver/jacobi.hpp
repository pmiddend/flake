#ifndef FLAKELIB_PLANAR_LAPLACE_SOLVER_JACOBI_HPP_INCLUDED
#define FLAKELIB_PLANAR_LAPLACE_SOLVER_JACOBI_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/planar/laplace_solver/base.hpp>
#include <flakelib/planar/laplace_solver/grid_scale.hpp>
#include <flakelib/planar/laplace_solver/iterations.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
{
namespace laplace_solver
{
class jacobi
:
	public planar::laplace_solver::base
{
FCPPT_NONCOPYABLE(
	jacobi);
public:
	explicit
	jacobi(
		flakelib::buffer_pool::object &,
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
		planar::laplace_solver::grid_scale const &,
		planar::laplace_solver::iterations const &);

	void
	solve(
		planar::laplace_solver::rhs const &,
		planar::laplace_solver::destination const &,
		planar::laplace_solver::initial_guess const &,
		planar::laplace_solver::boundary const &);

	planar::additional_scalar_data const &
	additional_scalar_data() const;

	~jacobi();
private:
	flakelib::buffer_pool::object &buffer_cache_;
	sge::opencl::command_queue::object &command_queue_;
	grid_scale::value_type const grid_scale_;
	iterations::value_type const iterations_;
	sge::opencl::program::object jacobi_program_;
	sge::opencl::kernel::object jacobi_kernel_;
	planar::additional_scalar_data additional_scalar_data_;
};
}
}
}

#endif
