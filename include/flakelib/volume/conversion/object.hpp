#ifndef FLAKELIB_VOLUME_CONVERSION_OBJECT_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_OBJECT_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer/volume_view_fwd.hpp>
#include <flakelib/volume/conversion/arrow_scale.hpp>
#include <flakelib/volume/conversion/cl_buffer.hpp>
#include <flakelib/volume/conversion/gl_buffer.hpp>
#include <flakelib/volume/conversion/grid_scale.hpp>
#include <flakelib/volume/conversion/multiplier.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/image/volume_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace volume
{
namespace conversion
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::opencl::command_queue::object &,
		flakelib::build_options const &);

	void
	to_arrow_vb(
		conversion::cl_buffer const &,
		conversion::gl_buffer const &,
		conversion::arrow_scale const &,
		conversion::grid_scale const &);

	void
	to_scalar_volume_texture(
		buffer::volume_view<cl_float> const &,
		sge::opencl::memory_object::image::volume &,
		conversion::multiplier const &);

	~object();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object to_arrow_vb_kernel_;
	sge::opencl::kernel::object to_scalar_volume_texture_kernel_;
};
}
}
}

#endif
