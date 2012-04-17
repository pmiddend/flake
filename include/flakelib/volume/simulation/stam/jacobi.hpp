#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_JACOBI_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_JACOBI_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel_scoped_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <flakelib/volume/float4_view.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <flakelib/volume/simulation/stam/initial_guess_buffer_view.hpp>
#include <flakelib/volume/simulation/stam/iterations.hpp>
#include <flakelib/volume/simulation/stam/rhs_buffer_view.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace volume
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
	jacobi(
		cl::program_context const &,
		flakelib::buffer_pool::object &,
		volume::simulation::stam::iterations const &);

	FLAKELIB_SYMBOL
	volume::unique_float_buffer_lock
	update(
		volume::simulation::stam::initial_guess_buffer_view const &,
		volume::boundary_buffer_view const &,
		volume::simulation::stam::rhs_buffer_view const &);

	FLAKELIB_SYMBOL
	~jacobi();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	cl::program program_;
	cl::kernel_scoped_ptr kernel_;
	volume::simulation::stam::iterations const iterations_;

};
}
}
}
}

#endif

