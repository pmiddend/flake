#ifndef FLAKELIB_VOLUME_LAPLACE_SOLVER_JACOBI_HPP_INCLUDED
#define FLAKELIB_VOLUME_LAPLACE_SOLVER_JACOBI_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/volume/laplace_solver/base.hpp>
#include <flakelib/volume/laplace_solver/grid_scale.hpp>
#include <flakelib/volume/laplace_solver/iterations.hpp>
#include <flakelib/volume/laplace_solver/boundary.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace volume
{
namespace laplace_solver
{
class jacobi
:
	public volume::laplace_solver::base
{
FCPPT_NONCOPYABLE(
	jacobi);
public:
	explicit
	jacobi(
		flakelib::buffer_pool::object &,
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
		volume::laplace_solver::boundary const &,
		volume::laplace_solver::grid_scale const &,
		volume::laplace_solver::iterations const &);

	void
	solve(
		volume::laplace_solver::rhs const &,
		volume::laplace_solver::destination const &,
		volume::laplace_solver::initial_guess const &);

	~jacobi();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	sge::opencl::command_queue::object &command_queue_;
	grid_scale::value_type const grid_scale_;
	iterations::value_type const iterations_;
	sge::opencl::program::object jacobi_program_;
	sge::opencl::kernel::object jacobi_kernel_;
};
}
}
}

#endif
