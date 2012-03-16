#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/splatter/rectangle/object.hpp>
#include <flakelib/splatter/pen/object_impl.hpp>
#include <sge/opencl/memory_object/buffer.hpp>


flakelib::splatter::object::object(
	cl::program_context const &_program_context)
:
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/splatter/apply.cl")),
		_program_context.compiler_flags()),
	splat_float_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"splat_float"))),
	splat_float2_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"splat_float2")))
{
}

void
flakelib::splatter::object::splat_planar_float(
	planar::float_view const &_buffer,
	splatter::pen::planar const &_pen,
	cl_float const _value)
{
	splat_float_->buffer_argument(
		"input",
		_buffer.buffer());

	splat_float_->numerical_argument(
		"pen_position_x",
		static_cast<cl_int>(
			_pen.bounding().position().get().x()));

	splat_float_->numerical_argument(
		"pen_position_y",
		static_cast<cl_int>(
			_pen.bounding().position().get().y()));

	splat_float_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_buffer.size().w()));

	splat_float_->numerical_argument(
		"value",
		_value);

	splat_float_->numerical_argument(
		"is_rectangle",
		static_cast<cl_int>(
			!_pen.is_round().get()));

	splat_float_->numerical_argument(
		"is_hard",
		static_cast<cl_int>(
			!_pen.is_smooth().get()));

	splat_float_->numerical_argument(
		"blend_factor",
		_pen.blend_factor().get());

	splat_float_->numerical_argument(
		"do_mixing",
		static_cast<cl_int>(
			_pen.draw_mode() == pen::draw_mode::mix));

	splat_float_->enqueue_automatic(
		_pen.bounding().size().get());
}

void
flakelib::splatter::object::splat_planar_float2(
	planar::float_view const &,
	splatter::pen::planar const &_pen,
	cl::float2 const &)
{
}

flakelib::splatter::object::~object()
{
}
