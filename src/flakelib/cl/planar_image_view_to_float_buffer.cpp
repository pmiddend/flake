#include <flakelib/cl/planar_image_view_to_float_buffer.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/image2d/view/make.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>

void
flakelib::cl::planar_image_view_to_float_buffer(
	sge::opencl::command_queue::object &_command_queue,
	sge::image2d::view::const_object const &_view,
	flakelib::buffer::planar_view<cl_float> const &_buffer)
{
	sge::opencl::command_queue::scoped_buffer_mapping scoped_bmap(
		_command_queue,
		_buffer.buffer(),
		CL_MAP_WRITE,
		sge::opencl::memory_object::byte_offset(
			0),
		sge::opencl::memory_object::byte_size(
			_buffer.buffer().byte_size()));

	sge::image2d::algorithm::copy_and_convert(
		_view,
		sge::image2d::view::make(
			static_cast<sge::image::raw_pointer>(
				scoped_bmap.ptr()),
			sge::image2d::view::size(
				_view),
			sge::image::color::format::r32f,
			sge::image2d::view::optional_pitch()),
		sge::image::algorithm::may_overlap::no);
}
