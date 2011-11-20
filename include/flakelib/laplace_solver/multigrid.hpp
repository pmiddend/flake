#ifndef FLAKELIB_LAPLACE_SOLVER_MULTIGRID_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_MULTIGRID_HPP_INCLUDED

#include <flakelib/planar_cache_fwd.hpp>
#include <flakelib/laplace_solver/base.hpp>
#include <flakelib/laplace_solver/debug_output.hpp>
#include <flakelib/laplace_solver/from.hpp>
#include <flakelib/laplace_solver/grid_scale.hpp>
#include <flakelib/laplace_solver/initial_guess.hpp>
#include <flakelib/laplace_solver/iterations.hpp>
#include <flakelib/laplace_solver/rhs.hpp>
#include <flakelib/laplace_solver/termination_size.hpp>
#include <flakelib/laplace_solver/to.hpp>
#include <flakelib/utility/object_fwd.hpp>
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
		flakelib::utility::object &,
		sge::opencl::command_queue::object &,
		laplace_solver::base &inner_solver,
		laplace_solver::grid_scale const &,
		laplace_solver::termination_size const &,
		laplace_solver::debug_output const &);

	void
	solve(
		laplace_solver::rhs const &,
		laplace_solver::destination const &,
		laplace_solver::initial_guess const &,
		laplace_solver::boundary const &);

	flakelib::additional_planar_data const &
	additional_planar_data() const;

	~multigrid();
private:
	flakelib::planar_cache &planar_cache_;
	flakelib::utility::object &utility_;
	sge::opencl::command_queue::object &command_queue_;
	laplace_solver::base &inner_solver_;
	grid_scale::value_type const grid_scale_;
	termination_size::value_type const termination_size_;
	bool const debug_output_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object laplacian_residual_kernel_;
	sge::opencl::kernel::object downsample_kernel_;
	sge::opencl::kernel::object upsample_kernel_;
	sge::opencl::kernel::object add_kernel_;
	flakelib::additional_planar_data additional_planar_data_;

	void
	laplacian_residual(
		laplace_solver::rhs const &,
		laplace_solver::from const &,
		laplace_solver::to const &,
		laplace_solver::boundary const &);

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

	void
	copy_to_planar_data(
		sge::opencl::memory_object::image::planar &,
		fcppt::string const &);
};
}
}

#endif
