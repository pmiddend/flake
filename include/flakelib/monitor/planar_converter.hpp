#ifndef FLAKELIB_MONITOR_PLANAR_CONVERTER_HPP_INCLUDED
#define FLAKELIB_MONITOR_PLANAR_CONVERTER_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/planar_object.hpp>
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
	to_vb(
		flakelib::planar_object const &,
		sge::opencl::memory_object::buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	to_texture(
		flakelib::planar_object const &,
		sge::opencl::memory_object::image::planar &,
		monitor::scaling_factor const &);

	~planar_converter();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object image_to_vb_kernel_;
	sge::opencl::kernel::object buffer_to_vb_kernel_;
	sge::opencl::kernel::object image_to_image_kernel_;
	sge::opencl::kernel::object buffer_to_image_kernel_;

	void
	image_to_vb(
		sge::opencl::memory_object::image::planar &,
		sge::opencl::memory_object::buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	planar_buffer_to_vb(
		flakelib::planar_buffer const &,
		sge::opencl::memory_object::buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	image_to_image(
		sge::opencl::memory_object::image::planar &,
		sge::opencl::memory_object::image::planar &,
		monitor::scaling_factor const &);

	void
	planar_buffer_to_image(
		flakelib::planar_buffer const &,
		sge::opencl::memory_object::image::planar &,
		monitor::scaling_factor const &);
};
}
}

#endif
