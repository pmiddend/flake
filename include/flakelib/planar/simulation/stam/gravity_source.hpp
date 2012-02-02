#ifndef FLAKELIB_PLANAR_SIMULATION_STAM_GRAVITY_SOURCE_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_STAM_GRAVITY_SOURCE_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/planar/simulation/stam/gravity_magnitude.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
{
namespace simulation
{
namespace stam
{
class gravity_source
{
FCPPT_NONCOPYABLE(
	gravity_source);
public:
	explicit
	gravity_source(
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
		stam::gravity_magnitude const &);

	void
	gravity_magnitude(
		stam::gravity_magnitude const &);

	void
	update(
		buffer::planar_view<cl_float2> const &,
		flakelib::duration const &);

	~gravity_source();
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
