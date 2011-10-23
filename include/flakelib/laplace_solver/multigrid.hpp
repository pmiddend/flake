#ifndef FLAKELIB_LAPLACE_SOLVER_MULTIGRID_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_MULTIGRID_HPP_INCLUDED

#include <flakelib/planar_cache_fwd.hpp>
#include <flakelib/laplace_solver/base.hpp>
#include <flakelib/laplace_solver/from.hpp>
#include <flakelib/laplace_solver/grid_scale.hpp>
#include <flakelib/laplace_solver/initial_guess.hpp>
#include <flakelib/laplace_solver/rhs.hpp>
#include <flakelib/laplace_solver/to.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace laplace_solver
{
class multigrid
:
	public laplace_solver::base
{
FCPPT_NONCOPYABLE(
	multigrid);
public:
	explicit
	multigrid(
		flakelib::planar_cache &,
		sge::opencl::command_queue::object &,
		laplace_solver::grid_scale const &);

	void
	solve(
		laplace_solver::rhs const &,
		laplace_solver::destination const &);

	~multigrid();
private:
	flakelib::planar_cache &planar_cache_;
	sge::opencl::command_queue::object &command_queue_;
	grid_scale::value_type const grid_scale_;
	unsigned const jacobi_iterations_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object null_image_kernel_;
	sge::opencl::kernel::object jacobi_kernel_;
	sge::opencl::kernel::object laplacian_residual_kernel_;
	sge::opencl::kernel::object downsample_kernel_;
	sge::opencl::kernel::object upsample_kernel_;
	sge::opencl::kernel::object add_kernel_;

	void
	null_image(
		sge::opencl::memory_object::image::planar &);

	void
	jacobi(
		laplace_solver::initial_guess const &,
		laplace_solver::destination const &,
		laplace_solver::rhs const &);

	void
	laplacian_residual(
		laplace_solver::rhs const &,
		laplace_solver::from const &,
		laplace_solver::to const &);

	void
	downsample(
		laplace_solver::from const &,
		laplace_solver::to const &);

	void
	upsample(
		laplace_solver::from const &,
		laplace_solver::to const &);

	void
	add(
		laplace_solver::from const &,
		laplace_solver::from const &,
		laplace_solver::to const &);
};
}
}

#endif
