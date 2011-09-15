#include "stam.hpp"
#include "../media_path_from_string.hpp"
#include <sge/image2d/view/size.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image/algorithm/may_overlap.hpp>
#include <sge/opencl/program/source_string_sequence.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/scoped_planar_mapping.hpp>
#include <sge/opencl/memory_object/rect.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/io/stream_to_string.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/text.hpp>

#include <fcppt/optional.hpp>

namespace
{
cl_image_format const
create_image_format(
	cl_channel_order const co,
	cl_channel_type const ct)
{
	cl_image_format result;
	result.image_channel_order = co;
	result.image_channel_data_type = ct;
	return result;
}
}

flake::simulation::stam::stam(
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_command_queue,
	flake::boundary_view const &boundary,
	sge::parse::json::object const &)
:
	v1_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_RG,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	v2_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_RG,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	p_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_RG,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	boundary_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_R,
			CL_UNORM_INT8),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	main_program_(
		_context,
		fcppt::assign::make_container<sge::opencl::program::source_string_sequence>(
			fcppt::io::stream_to_string(
				*fcppt::make_unique_ptr<fcppt::io::cifstream>(
					flake::media_path_from_string(
						FCPPT_TEXT("kernels/stam.cl"))))),
		sge::opencl::program::build_parameters()),
	reset_vector_field_(
		main_program_,
		sge::opencl::kernel::name(
			"reset_vector_field"))
{
	fcppt::io::cout << FCPPT_TEXT("Resetting vector field\n");
	sge::image2d::dim const boundary_dim =
		sge::image2d::view::size(
			boundary.get());

	reset_vector_field_.argument(
		sge::opencl::kernel::argument_index(
			0),
		v1_);

	sge::opencl::command_queue::enqueue_kernel(
		_command_queue,
		reset_vector_field_,
		fcppt::assign::make_array<std::size_t>
			(boundary_dim[0])
			(boundary_dim[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());

	/*
	sge::opencl::command_queue::scoped_planar_mapping scoped_image(
		_command_queue,
		boundary_,
		CL_MAP_WRITE,
		sge::opencl::memory_object::rect(
			sge::opencl::memory_object::rect::vector::null(),
			fcppt::math::dim::structure_cast<sge::opencl::memory_object::rect::dim>(
				boundary_dim)));

	sge::image2d::algorithm::copy_and_convert(
		boundary.get(),
		scoped_image.view(),
		sge::image::algorithm::may_overlap::no);
	*/
}

sge::opencl::memory_object::image::planar &
flake::simulation::stam::vector_field()
{
	return v1_;
}

void
flake::simulation::stam::update(
	flake::duration const &)
{
}

flake::simulation::stam::~stam()
{
}
