#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/image/volume.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>


flakelib::volume::conversion::object::object(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options)
:
	command_queue_(
		_command_queue),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/volume/conversion.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	to_arrow_vb_kernel_(
		program_,
		sge::opencl::kernel::name(
			"to_arrow_vb")),
	to_scalar_volume_texture_kernel_(
		program_,
		sge::opencl::kernel::name(
			"to_scalar_volume_texture"))
{
}

void
flakelib::volume::conversion::object::to_arrow_vb(
	conversion::cl_buffer const &_cl_buffer,
	conversion::gl_buffer const &_gl_buffer,
	conversion::arrow_scale const &_arrow_scale,
	conversion::grid_scale const &_grid_scale)
{
	to_arrow_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_cl_buffer.get().buffer());

	to_arrow_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_gl_buffer.get());

	to_arrow_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_arrow_scale.get());

	to_arrow_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		_grid_scale.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_gl_buffer.get());

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		to_arrow_vb_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_cl_buffer.get().size()[0])
			(_cl_buffer.get().size()[1])
			(_cl_buffer.get().size()[2]).container());
}

void
flakelib::volume::conversion::object::to_scalar_volume_texture(
	buffer::volume_view<cl_float> const &_v,
	sge::opencl::memory_object::image::volume &_tex,
	conversion::multiplier const &_multiplier)
{
	to_scalar_volume_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_v.buffer());

	to_scalar_volume_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_tex);

	to_scalar_volume_texture_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_multiplier.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_tex);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		to_scalar_volume_texture_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_v.size()[0])
			(_v.size()[1])
			(_v.size()[2]).container());
}

flakelib::volume::conversion::object::~object()
{
}
