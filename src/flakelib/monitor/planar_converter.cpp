#include <flakelib/media_path_from_string.hpp>
#include <flakelib/monitor/planar_converter.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/variant/holds_type.hpp>


flakelib::monitor::planar_converter::planar_converter(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options)
:
	command_queue_(
		_command_queue),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/vertex_buffer_conversions.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	to_arrow_vb_kernel_(
		program_,
		sge::opencl::kernel::name(
			"to_arrow_vb")),
	scalar_to_texture_kernel_(
		program_,
		sge::opencl::kernel::name(
			"scalar_to_texture"))
{
}

void
flakelib::monitor::planar_converter::to_arrow_vb(
	buffer::planar_view<cl_float2> const &_planar_buffer,
	sge::opencl::memory_object::buffer &_vb,
	monitor::grid_scale const &_grid_scale,
	monitor::arrow_scale const &_arrow_scale)
{
	to_arrow_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_vb);

	to_arrow_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_planar_buffer.buffer());

	to_arrow_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_uint>(
			_planar_buffer.size().w()));

	to_arrow_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		_grid_scale.get());

	to_arrow_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		_arrow_scale.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_vb);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		to_arrow_vb_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_planar_buffer.size().w())
			(_planar_buffer.size().h()).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(64)
			(1).container());
}

void
flakelib::monitor::planar_converter::scalar_to_texture(
	buffer::planar_view<cl_float> const &_planar_buffer,
	sge::opencl::memory_object::image::planar &_texture,
	monitor::scaling_factor const &_scaling)
{
	FCPPT_ASSERT_PRE(
		_planar_buffer.size() == _texture.size());

	scalar_to_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_planar_buffer.buffer());

	scalar_to_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_texture);

	scalar_to_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_scaling.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_texture);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		scalar_to_texture_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_planar_buffer.size().w())
			(_planar_buffer.size().h()).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(64)
			(1).container());
}

flakelib::monitor::planar_converter::~planar_converter()
{
}
