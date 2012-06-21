#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/splatter/box/object.hpp>
#include <flakelib/splatter/pen/object_impl.hpp>
#include <flakelib/splatter/rectangle/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/assert/unimplemented_message.hpp>


flakelib::splatter::object::object(
	cl::program_context const &_program_context)
:
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/splatter/apply.cl")),
		_program_context.compiler_flags()),
	splat_planar_float_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"splat_planar_float"))),
	splat_planar_float2_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"splat_planar_float2"))),
	splat_volume_float_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"splat_volume_float")))
{
}

void
flakelib::splatter::object::splat_planar_float(
	planar::float_view const &_buffer,
	splatter::pen::planar const &_pen,
	cl_float const _value)
{
	splat_planar_float_->buffer_argument(
		"input",
		_buffer.buffer());

	splat_planar_float_->numerical_argument(
		"pen_position_x",
		static_cast<cl_int>(
			_pen.bounding().position().get().x()));

	splat_planar_float_->numerical_argument(
		"pen_position_y",
		static_cast<cl_int>(
			_pen.bounding().position().get().y()));

	splat_planar_float_->numerical_argument(
		"buffer_width",
		static_cast<cl_int>(
			_buffer.size().w()));

	splat_planar_float_->numerical_argument(
		"buffer_height",
		static_cast<cl_int>(
			_buffer.size().h()));

	splat_planar_float_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_buffer.size().w()));

	splat_planar_float_->numerical_argument(
		"value",
		_value);

	splat_planar_float_->numerical_argument(
		"is_rectangle",
		static_cast<cl_int>(
			!_pen.is_round().get()));

	splat_planar_float_->numerical_argument(
		"is_hard",
		static_cast<cl_int>(
			!_pen.is_smooth().get()));

	splat_planar_float_->numerical_argument(
		"blend_factor",
		_pen.blend_factor().get());

	splat_planar_float_->numerical_argument(
		"do_mixing",
		static_cast<cl_int>(
			_pen.draw_mode() == pen::draw_mode::mix));

	splat_planar_float_->enqueue_automatic(
		sge::opencl::command_queue::global_dim2(
			_pen.bounding().size().get()));
}

void
flakelib::splatter::object::splat_planar_float2(
	planar::float_view const &,
	splatter::pen::planar const &,
	cl::float2 const &)
{
	FCPPT_ASSERT_UNIMPLEMENTED_MESSAGE(
		FCPPT_TEXT("splatting planar float2 is not supported yet"));
}

void
flakelib::splatter::object::splat_volume_float(
	volume::float_view const &_buffer,
	splatter::pen::volume const &_pen,
	cl_float const _value)
{
	splat_volume_float_->buffer_argument(
		"input",
		_buffer.buffer());

	splat_volume_float_->numerical_argument(
		"pen_position_x",
		static_cast<cl_int>(
			_pen.bounding().position().get().x()));

	splat_volume_float_->numerical_argument(
		"pen_position_y",
		static_cast<cl_int>(
			_pen.bounding().position().get().y()));

	splat_volume_float_->numerical_argument(
		"pen_position_z",
		static_cast<cl_int>(
			_pen.bounding().position().get().z()));

	splat_volume_float_->numerical_argument(
		"buffer_width",
		static_cast<cl_int>(
			_buffer.size().w()));

	splat_volume_float_->numerical_argument(
		"buffer_height",
		static_cast<cl_int>(
			_buffer.size().h()));

	splat_volume_float_->numerical_argument(
		"buffer_depth",
		static_cast<cl_int>(
			_buffer.size().d()));

	splat_volume_float_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_buffer.size().w()));

	splat_volume_float_->numerical_argument(
		"value",
		_value);

	splat_volume_float_->numerical_argument(
		"is_box",
		static_cast<cl_int>(
			!_pen.is_round().get()));

	splat_volume_float_->numerical_argument(
		"is_hard",
		static_cast<cl_int>(
			!_pen.is_smooth().get()));

	splat_volume_float_->numerical_argument(
		"blend_factor",
		_pen.blend_factor().get());

	splat_volume_float_->numerical_argument(
		"do_mixing",
		static_cast<cl_int>(
			_pen.draw_mode() == pen::draw_mode::mix));

	splat_volume_float_->enqueue_automatic(
		sge::opencl::command_queue::global_dim3(
			_pen.bounding().size().get()));
}

flakelib::splatter::object::~object()
{
}
