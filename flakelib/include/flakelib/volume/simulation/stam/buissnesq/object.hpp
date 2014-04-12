#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_BUISSNESQ_OBJECT_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_BUISSNESQ_OBJECT_HPP_INCLUDED

#include <flakelib/duration.hpp>
#include <flakelib/symbol.hpp>
#include <flakelib/cl/kernel_unique_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <flakelib/volume/simulation/stam/velocity.hpp>
#include <flakelib/volume/simulation/stam/buissnesq/ambient_temperature.hpp>
#include <flakelib/volume/simulation/stam/buissnesq/density_strength.hpp>
#include <flakelib/volume/simulation/stam/buissnesq/density_view.hpp>
#include <flakelib/volume/simulation/stam/buissnesq/temperature_strength.hpp>
#include <flakelib/volume/simulation/stam/buissnesq/temperature_view.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace volume
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
	explicit
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
	cl::kernel_unique_ptr const kernel_;
};
}
}
}
}
}

#endif

