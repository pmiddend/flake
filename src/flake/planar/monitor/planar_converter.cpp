#include <flake/planar/monitor/planar_converter.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
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


flake::planar::monitor::planar_converter::planar_converter(
	flakelib::cl::program_context const &_program_context)
:
	command_queue_(
		_program_context.command_queue()),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flake/planar/vertex_buffer_conversions.cl")),
		_program_context.compiler_flags()),
	to_arrow_vb_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"to_arrow_vb"))),
	scalar_to_texture_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"scalar_to_texture")))
{
}

void
flake::planar::monitor::planar_converter::to_arrow_vb(
	flakelib::planar::float2_view const &_planar_buffer,
	sge::opencl::memory_object::buffer &_vb,
	monitor::grid_scale const &_grid_scale,
	monitor::arrow_scale const &_arrow_scale)
{
	to_arrow_vb_kernel_->buffer_argument(
		"vb",
		_vb);

	to_arrow_vb_kernel_->buffer_argument(
		"buffer",
		_planar_buffer.buffer());

	to_arrow_vb_kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_planar_buffer.size().w()));

	to_arrow_vb_kernel_->numerical_argument(
		"grid_scale",
		_grid_scale.get());

	to_arrow_vb_kernel_->numerical_argument(
		"arrow_scale",
		_arrow_scale.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_vb);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	to_arrow_vb_kernel_->enqueue_automatic(
		_planar_buffer.size());
}

void
flake::planar::monitor::planar_converter::scalar_to_texture(
	flakelib::planar::float_view const &_planar_buffer,
	sge::opencl::memory_object::image::planar &_texture,
	monitor::scaling_factor const &_scaling)
{
	FCPPT_ASSERT_PRE(
		_planar_buffer.size() == _texture.size());

	scalar_to_texture_kernel_->buffer_argument(
		"input",
		_planar_buffer.buffer());

	scalar_to_texture_kernel_->buffer_argument(
		"output",
		_texture);

	scalar_to_texture_kernel_->numerical_argument(
		"scaling",
		_scaling.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_texture);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	scalar_to_texture_kernel_->enqueue_automatic(
		_planar_buffer.size());
}

flake::planar::monitor::planar_converter::~planar_converter()
{
}
