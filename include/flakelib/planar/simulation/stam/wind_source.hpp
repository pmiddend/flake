#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_WIND_SOURCE_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_WIND_SOURCE_HPP_INCLUDED

#include <flakelib/planar/simulation/stam/external_force_magnitude.hpp>
#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/build_options.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/opencl/kernel/object.hpp>
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
	explicit
	wind_source(
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
		stam::external_force_magnitude const &);

	void
	external_force_magnitude(
		stam::external_force_magnitude const &);

	void
	update(
		buffer::planar_view<cl_float2> const &);

	~wind_source();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object kernel_;
};
}
}
}
}

#endif
