#ifndef FLAKELIB_MONITOR_PLANAR_CONVERTER_HPP_INCLUDED
#define FLAKELIB_MONITOR_PLANAR_CONVERTER_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/monitor/arrow_scale.hpp>
#include <flakelib/monitor/grid_scale.hpp>
#include <flakelib/monitor/scaling_factor.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/opencl/program/object.hpp>


namespace flakelib
{
namespace monitor
{
class planar_converter
{
FCPPT_NONCOPYABLE(
	planar_converter);
public:
	explicit
	planar_converter(
		sge::opencl::command_queue::object &,
		flakelib::build_options const &);

	void
	to_arrow_vb(
		buffer::planar_view<cl_float> const &,
		// Technically a planar buffer as well, but one coming from
		// OpenGL.
		sge::opencl::memory_object::buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	scalar_to_texture(
		buffer::planar_view<cl_float> const &,
		sge::opencl::memory_object::image::planar &,
		monitor::scaling_factor const &);

	~planar_converter();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object to_arrow_vb_kernel_;
	sge::opencl::kernel::object scalar_to_texture_kernel_;
};
}
}

#endif
