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
#include <sge/image2d/view/const_element.hpp>
#include <sge/image/color/l8_format.hpp>
#include <sge/image/color/r32f_format.hpp>
#include <mizuiro/image/algorithm/binary_iteration.hpp>
#include <mizuiro/color/channel/red.hpp>
#include <mizuiro/color/channel/luminance.hpp>
#include <fcppt/variant/object.hpp>

namespace
{
struct convert
{
	typedef
	void
	result_type;

	template<typename Src,typename Dest>
	result_type
	operator()(
		Src const &_src,
		Dest const &_dest) const
	{
		_dest.set(
			mizuiro::color::channel::red(),
			_src.get(mizuiro::color::channel::luminance()) > 0 ? 1.0f : 0.0f
			/*
			static_cast<float>(
				_src.get(
					mizuiro::color::channel::luminance())/
			255.f)*/);
	}
};
}

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

	typedef
	sge::image2d::view::const_element<sge::image::color::l8_format>::type
	l8_view_type;

	typedef
	sge::image2d::view::element<sge::image::color::r32f_format>::type
	r32f_view_type;

	l8_view_type const l8_view =
		_view.get().get<l8_view_type>();

	r32f_view_type const r32f_view =
		sge::image2d::view::make(
			static_cast<sge::image::raw_pointer>(
				scoped_bmap.ptr()),
			sge::image2d::view::size(
				_view),
			sge::image::color::format::r32f,
			sge::image2d::view::optional_pitch()).get().get<r32f_view_type>();

	mizuiro::image::algorithm::binary_iteration(
		convert(),
		l8_view,
		r32f_view);
	/*
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
		*/
}
