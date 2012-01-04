#ifndef FLAKELIB_PLANAR_LAPLACE_SOLVER_MULTIGRID_HPP_INCLUDED
#define FLAKELIB_PLANAR_LAPLACE_SOLVER_MULTIGRID_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/buffer_pool/planar_lock_fwd.hpp>
#include <flakelib/planar/laplace_solver/base.hpp>
#include <flakelib/planar/laplace_solver/debug_output.hpp>
#include <flakelib/planar/laplace_solver/from.hpp>
#include <flakelib/planar/laplace_solver/grid_scale.hpp>
#include <flakelib/planar/laplace_solver/initial_guess.hpp>
#include <flakelib/planar/laplace_solver/iterations.hpp>
#include <flakelib/planar/laplace_solver/rhs.hpp>
#include <flakelib/planar/laplace_solver/termination_size.hpp>
#include <flakelib/planar/laplace_solver/to.hpp>
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
namespace planar
{
namespace laplace_solver
{
class multigrid
:
	public planar::laplace_solver::base
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
		planar::laplace_solver::base &inner_solver,
		planar::laplace_solver::grid_scale const &,
		planar::laplace_solver::termination_size const &,
		planar::laplace_solver::debug_output const &);

	void
	solve(
		planar::laplace_solver::rhs const &,
		planar::laplace_solver::destination const &,
		planar::laplace_solver::initial_guess const &,
		planar::laplace_solver::boundary const &);

	planar::additional_scalar_data const &
	additional_scalar_data() const;

	~multigrid();
private:
	typedef
	boost::ptr_vector<flakelib::buffer_pool::planar_lock<cl_float> >
	debug_buffer_sequence;

	flakelib::buffer_pool::object &buffer_cache_;
	flakelib::utility::object &utility_;
	sge::opencl::command_queue::object &command_queue_;
	planar::laplace_solver::base &inner_solver_;
	grid_scale::value_type const grid_scale_;
	termination_size::value_type const termination_size_;
	bool const debug_output_;
	sge::opencl::program::object main_program_;
	sge::opencl::kernel::object laplacian_residual_kernel_;
	sge::opencl::kernel::object downsample_kernel_;
	sge::opencl::kernel::object upsample_kernel_;
	sge::opencl::kernel::object add_kernel_;
	debug_buffer_sequence debug_buffers_;
	planar::additional_scalar_data additional_scalar_data_;

	void
	laplacian_residual(
		planar::laplace_solver::rhs const &,
		planar::laplace_solver::from const &,
		planar::laplace_solver::to const &,
		planar::laplace_solver::boundary const &);

	void
	downsample(
		planar::laplace_solver::from const &,
		planar::laplace_solver::to const &);

	void
	upsample(
		planar::laplace_solver::from const &,
		planar::laplace_solver::to const &);

	void
	add(
		planar::laplace_solver::from const &,
		planar::laplace_solver::from const &,
		planar::laplace_solver::to const &);

	void
	copy_to_planar_data(
		buffer::planar_view<cl_float> const &,
		fcppt::string const &);
};
}
}
}

#endif
