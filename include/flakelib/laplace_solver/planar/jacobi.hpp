#ifndef FLAKELIB_LAPLACE_SOLVER_PLANAR_JACOBI_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_PLANAR_JACOBI_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/laplace_solver/planar/base.hpp>
#include <flakelib/laplace_solver/planar/grid_scale.hpp>
#include <flakelib/laplace_solver/planar/iterations.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace laplace_solver
{
namespace planar
{
class jacobi
:
	public laplace_solver::planar::base
{
FCPPT_NONCOPYABLE(
	jacobi);
public:
	explicit
	jacobi(
		flakelib::buffer_pool::object &,
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
		laplace_solver::planar::grid_scale const &,
		laplace_solver::planar::iterations const &);

	void
	solve(
		laplace_solver::planar::rhs const &,
		laplace_solver::planar::destination const &,
		laplace_solver::planar::initial_guess const &,
		laplace_solver::planar::boundary const &);

	flakelib::additional_planar_data const &
	additional_planar_data() const;

	~jacobi();
private:
	flakelib::buffer_pool::object &buffer_cache_;
	sge::opencl::command_queue::object &command_queue_;
	grid_scale::value_type const grid_scale_;
	iterations::value_type const iterations_;
	sge::opencl::program::object jacobi_program_;
	sge::opencl::kernel::object jacobi_kernel_;
	flakelib::additional_planar_data additional_planar_data_;
};
}
}
}

#endif
