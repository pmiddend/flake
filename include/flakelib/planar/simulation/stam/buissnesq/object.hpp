#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_BUISSNESQ_OBJECT_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_BUISSNESQ_OBJECT_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/symbol.hpp>
#include <flakelib/cl/kernel_fwd.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/simulation/stam/velocity.hpp>
#include <flakelib/planar/simulation/stam/buissnesq/ambient_temperature.hpp>
#include <flakelib/planar/simulation/stam/buissnesq/density_strength.hpp>
#include <flakelib/planar/simulation/stam/buissnesq/density_view.hpp>
#include <flakelib/planar/simulation/stam/buissnesq/temperature_strength.hpp>
#include <flakelib/planar/simulation/stam/buissnesq/temperature_view.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
namespace buissnesq
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	FLAKELIB_SYMBOL
	object(
		cl::program_context const &,
		buissnesq::density_strength const &,
		buissnesq::temperature_strength const &,
		buissnesq::ambient_temperature const &);

	FLAKELIB_SYMBOL
	void
	update(
		stam::velocity const &,
		buissnesq::density_view const &,
		buissnesq::temperature_view const &,
		flakelib::duration const &);

	FLAKELIB_SYMBOL
	~object();
private:
	cl::program program_;
	cl::unique_kernel_ptr kernel_;
};
}
}
}
}
}

#endif

