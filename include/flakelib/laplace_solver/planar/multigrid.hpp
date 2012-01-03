#ifndef FLAKELIB_LAPLACE_SOLVER_PLANAR_MULTIGRID_HPP_INCLUDED
#define FLAKELIB_LAPLACE_SOLVER_PLANAR_MULTIGRID_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/buffer_pool/planar_lock_fwd.hpp>
#include <flakelib/laplace_solver/planar/base.hpp>
#include <flakelib/laplace_solver/planar/debug_output.hpp>
#include <flakelib/laplace_solver/planar/from.hpp>
#include <flakelib/laplace_solver/planar/grid_scale.hpp>
#include <flakelib/laplace_solver/planar/initial_guess.hpp>
#include <flakelib/laplace_solver/planar/iterations.hpp>
#include <flakelib/laplace_solver/planar/rhs.hpp>
#include <flakelib/laplace_solver/planar/termination_size.hpp>
#include <flakelib/laplace_solver/planar/to.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace laplace_solver
{
namespace planar
{
class multigrid
:
	public laplace_solver::planar::base
{
FCPPT_NONCOPYABLE(
	multigrid);
public:
	explicit
	multigrid(
		flakelib::buffer_pool::object &,
		flakelib::utility::object &,
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
		laplace_solver::planar::base &inner_solver,
		laplace_solver::planar::grid_scale const &,
		laplace_solver::planar::termination_size const &,
		laplace_solver::planar::debug_output const &);

	void
	solve(
		laplace_solver::planar::rhs const &,
		laplace_solver::planar::destination const &,
		laplace_solver::planar::initial_guess const &,
		laplace_solver::planar::boundary const &);

	flakelib::additional_planar_data const &
	additional_planar_data() const;

	~multigrid();
private:
	typedef
	boost::ptr_vector<flakelib::buffer_pool::planar_lock<cl_float> >
	debug_buffer_sequence;

	flakelib::buffer_pool::object &buffer_cache_;
	flakelib::utility::object &utility_;
	sge::opencl::command_queue::object &command_queue_;
	laplace_solver::planar::base &inner_solver_;
	grid_scale::value_type const grid_scale_;
	termination_size::value_type const termination_size_;
	bool const debug_output_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object laplacian_residual_kernel_;
	sge::opencl::kernel::object downsample_kernel_;
	sge::opencl::kernel::object upsample_kernel_;
	sge::opencl::kernel::object add_kernel_;
	debug_buffer_sequence debug_buffers_;
	flakelib::additional_planar_data additional_planar_data_;

	void
	laplacian_residual(
		laplace_solver::planar::rhs const &,
		laplace_solver::planar::from const &,
		laplace_solver::planar::to const &,
		laplace_solver::planar::boundary const &);

	void
	downsample(
		laplace_solver::planar::from const &,
		laplace_solver::planar::to const &);

	void
	upsample(
		laplace_solver::planar::from const &,
		laplace_solver::planar::to const &);

	void
	add(
		laplace_solver::planar::from const &,
		laplace_solver::planar::from const &,
		laplace_solver::planar::to const &);

	void
	copy_to_planar_data(
		buffer::planar_view<cl_float> const &,
		fcppt::string const &);
};
}
}
}

#endif
