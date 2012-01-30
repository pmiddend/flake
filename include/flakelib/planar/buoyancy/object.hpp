#ifndef FLAKELIB_PLANAR_BUOYANCY_OBJECT_HPP_INCLUDED
#define FLAKELIB_PLANAR_BUOYANCY_OBJECT_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/planar/buoyancy/ambient_temperature.hpp>
#include <flakelib/planar/buoyancy/temperature_strength.hpp>
#include <flakelib/planar/buoyancy/density_strength.hpp>
#include <flakelib/planar/buoyancy/velocity.hpp>
#include <flakelib/planar/buoyancy/temperature.hpp>
#include <flakelib/planar/buoyancy/density.hpp>
#include <flakelib/planar/buoyancy/boundary.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace planar
{
namespace buoyancy
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
//		buoyancy::boundary const &,
		buoyancy::temperature_strength const &,
		buoyancy::density_strength const &,
		buoyancy::ambient_temperature const &);

	void
	temperature_strength(
		cl_float);

	void
	density_strength(
		cl_float);

	void
	ambient_temperature(
		cl_float);

	void
	update(
		buoyancy::velocity const &,
		buoyancy::temperature const &,
		buoyancy::density const &,
		flakelib::duration const &);

	~object();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object kernel_;
	cl_float temperature_strength_;
	cl_float density_strength_;
	cl_float ambient_temperature_;
};
}
}
}

#endif

