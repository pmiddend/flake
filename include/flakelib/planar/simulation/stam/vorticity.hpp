#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_VORTICITY_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_VORTICITY_HPP_INCLUDED

#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/planar/unique_float_buffer_lock.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/planar/simulation/stam/velocity.hpp>
#include <fcppt/noncopyable.hpp>
#include <flakelib/symbol.hpp>

namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
class vorticity
{
FCPPT_NONCOPYABLE(
	vorticity);
public:
	FLAKELIB_SYMBOL
	explicit
	vorticity(
		cl::program_context const &,
		flakelib::buffer_pool::object &);

	FLAKELIB_SYMBOL
	planar::unique_float_buffer_lock
	apply_vorticity(
		planar::boundary_buffer_view const &,
		stam::velocity const &);

	FLAKELIB_SYMBOL
	~vorticity();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	cl::program program_;
	cl::unique_kernel_ptr vorticity_kernel_;
	//cl::unique_kernel_ptr confinement_kernel_;
};
}
}
}
}

#endif
