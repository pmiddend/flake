#include <flakelib/cl/planar_image_view_to_cl_image.hpp>
#include <sge/image/algorithm/may_overlap.hpp>
#include <sge/image/algorithm/uninitialized.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image2d/view/format.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/opencl/command_queue/map_flags.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/command_queue/scoped_planar_mapping.hpp>
#include <sge/opencl/event/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/memory_object/image/sge_color_format_to_opencl.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/cast/size_fun.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/null.hpp>


fcppt::unique_ptr<sge::opencl::memory_object::image::planar>
flakelib::cl::planar_image_view_to_cl_image(
	sge::image2d::view::const_object const &_planar_image,
	sge::opencl::command_queue::object &_command_queue)
{
	fcppt::unique_ptr<sge::opencl::memory_object::image::planar> result(
		fcppt::make_unique_ptr<sge::opencl::memory_object::image::planar>(
			_command_queue.context(),
			sge::opencl::memory_object::flags_field{
				sge::opencl::memory_object::flags::read,
				sge::opencl::memory_object::flags::write},
			sge::opencl::memory_object::image::sge_color_format_to_opencl(
				sge::image2d::view::format(
					_planar_image)),
			fcppt::math::dim::structure_cast<
				sge::opencl::dim2,
				fcppt::cast::size_fun>(
				sge::image2d::view::size(
					_planar_image)),
			sge::opencl::memory_object::image::planar_pitch(
				0u)));

	sge::opencl::command_queue::scoped_planar_mapping scoped_image(
		_command_queue,
		*result,
		sge::opencl::command_queue::map_flags::write,
		sge::opencl::memory_object::rect(
			fcppt::math::vector::null<
				sge::opencl::memory_object::rect::vector
			>(),
			fcppt::math::dim::structure_cast<
				sge::opencl::memory_object::rect::dim,
				fcppt::cast::size_fun>(
				sge::image2d::view::size(
					_planar_image))),
		sge::opencl::event::sequence());

	sge::image2d::algorithm::copy_and_convert(
		_planar_image,
		scoped_image.view(),
		sge::image::algorithm::may_overlap::no,
		sge::image::algorithm::uninitialized::yes);

	return
		result;
}
