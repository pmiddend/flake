#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_VORTICITY_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_VORTICITY_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/planar/boundary_buffer_view.hpp>
#include <flakelib/planar/float2_view.hpp>
#include <flakelib/planar/simulation/stam/vorticity_strength.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>


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
public:
	explicit
	vorticity(
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
		buffer_pool::object &,
		planar::boundary_buffer_view const &,
		stam::vorticity_strength const &);

	void
	vorticity_strength(
		cl_float);

	void
	update(
		planar::float2_view const &,
		flakelib::duration const &);

	~vorticity();
private:
	sge::opencl::command_queue::object &command_queue_;
	buffer_pool::object &buffer_pool_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object vorticity_kernel_;
	sge::opencl::kernel::object gradient_and_cross_kernel_;
};
}
}
}
}

#endif

