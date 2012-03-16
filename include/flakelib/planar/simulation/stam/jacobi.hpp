#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_JACOBI_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_JACOBI_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel_fwd.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/unique_float_buffer_lock.hpp>
#include <flakelib/planar/simulation/stam/initial_guess_buffer_view.hpp>
#include <flakelib/planar/simulation/stam/iterations.hpp>
#include <flakelib/planar/simulation/stam/rhs_buffer_view.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
class jacobi
{
FCPPT_NONCOPYABLE(
	jacobi);
public:
	FLAKELIB_SYMBOL
	explicit
	jacobi(
		cl::program_context const &,
		flakelib::buffer_pool::object &,
		planar::simulation::stam::iterations const &);

	FLAKELIB_SYMBOL
	planar::unique_float_buffer_lock
	update(
		planar::simulation::stam::initial_guess_buffer_view const &,
		planar::boundary_buffer_view const &,
		planar::simulation::stam::rhs_buffer_view const &);

	FLAKELIB_SYMBOL
	~jacobi();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	cl::program program_;
	cl::unique_kernel_ptr kernel_;
	planar::simulation::stam::iterations const iterations_;

};
}
}
}
}

#endif

