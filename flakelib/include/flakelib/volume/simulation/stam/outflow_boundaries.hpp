#ifndef FLAKELIB_VOLUME_SIMULATION_STAM_OUTFLOW_BOUNDARIES_HPP_INCLUDED
#define FLAKELIB_VOLUME_SIMULATION_STAM_OUTFLOW_BOUNDARIES_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/cl/kernel_unique_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/volume/boundary_buffer_view.hpp>
#include <flakelib/volume/float4_view.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace volume
{
namespace simulation
{
namespace stam
{
class outflow_boundaries
{
FCPPT_NONCOPYABLE(
	outflow_boundaries);
public:
	FLAKELIB_SYMBOL
	explicit
	outflow_boundaries(
		cl::program_context const &);

	FLAKELIB_SYMBOL
	void
	update(
		volume::float4_view const &);

	FLAKELIB_SYMBOL
	~outflow_boundaries();
private:
	cl::program program_;
	cl::kernel_unique_ptr const kernel_;

};
}
}
}
}

#endif

