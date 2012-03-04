#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_SUBTRACT_PRESSURE_GRADIENT_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_SUBTRACT_PRESSURE_GRADIENT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/cl/kernel_fwd.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/planar/velocity_buffer_view.hpp>
#include <flakelib/planar/simulation/stam/pressure_buffer_view.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
class subtract_pressure_gradient
{
FCPPT_NONCOPYABLE(
	subtract_pressure_gradient);
public:
	FLAKELIB_SYMBOL
	explicit
	subtract_pressure_gradient(
		cl::program_context const &);

	FLAKELIB_SYMBOL
	void
	update(
		planar::velocity_buffer_view const &,
		planar::boundary_buffer_view const &,
		planar::simulation::stam::pressure_buffer_view const &);

	FLAKELIB_SYMBOL
	~subtract_pressure_gradient();
private:
	cl::program program_;
	cl::unique_kernel_ptr kernel_;

};
}
}
}
}

#endif

