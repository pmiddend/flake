#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_SUBTRACT_PRESSURE_GRADIENT_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_SUBTRACT_PRESSURE_GRADIENT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/cl/kernel_unique_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <flakelib/volume/velocity_buffer_view.hpp>
#include <flakelib/volume/simulation/stam/pressure_buffer_view.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace volume
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
		volume::velocity_buffer_view const &,
		volume::boundary_buffer_view const &,
		volume::simulation::stam::pressure_buffer_view const &);

	FLAKELIB_SYMBOL
	~subtract_pressure_gradient();
private:
	cl::program program_;
	cl::kernel_unique_ptr const kernel_;

};
}
}
}
}

#endif

