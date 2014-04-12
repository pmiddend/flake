#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_WIND_SOURCE_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_WIND_SOURCE_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/cl/kernel_unique_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/simulation/stam/wind_strength.hpp>
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
class wind_source
{
FCPPT_NONCOPYABLE(
	wind_source);
public:
	FLAKELIB_SYMBOL
	wind_source(
		cl::program_context const &,
		stam::wind_strength const &);

	FLAKELIB_SYMBOL
	void
	wind_strength(
		cl_float);

	FLAKELIB_SYMBOL
	void
	update(
		planar::float2_view const &);

	FLAKELIB_SYMBOL
	~wind_source();
private:
	cl::program program_;
	cl::kernel_unique_ptr const kernel_;
};
}
}
}
}

#endif
