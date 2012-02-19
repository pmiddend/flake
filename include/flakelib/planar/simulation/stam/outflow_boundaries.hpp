#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_OUTFLOW_BOUNDARIES_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_OUTFLOW_BOUNDARIES_HPP_INCLUDED

#include <flakelib/cl/kernel_fwd.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/planar/program_context_fwd.hpp>
#include <flakelib/planar/velocity_buffer_view.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace planar
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
	explicit
	outflow_boundaries(
		planar::program_context const &);

	void
	update(
		planar::float2_view const &);

	~outflow_boundaries();
private:
	cl::program program_;
	cl::unique_kernel_ptr kernel_;

};
}
}
}
}

#endif

