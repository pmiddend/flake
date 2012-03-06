#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/splatter/rectangle/object.hpp>
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
	splatter::rectangle::object const &_rectangle,
	cl_float const _value,
	splatter::pen_type::type const _pen,
	splatter::hardness::type const _hardness,
	splatter::mix_mode::type const _mix_mode)
{
	splat_float_->buffer_argument(
		"input",
		_buffer.buffer());

	splat_float_->numerical_argument(
		"pen_position_x",
		static_cast<cl_int>(
			_rectangle.position().get().w()));

	splat_float_->numerical_argument(
		"pen_position_y",
		static_cast<cl_int>(
			_rectangle.position().get().h()));

	splat_float_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_buffer.size().w()));

	splat_float_->numerical_argument(
		"value",
		_value);

	switch(_pen)
	{
		case splatter::pen_type::round:
			splat_float_->numerical_argument(
				"is_rectangle",
				static_cast<cl_int>(
					false));
			break;
		case splatter::pen_type::square:
			splat_float_->numerical_argument(
				"is_rectangle",
				static_cast<cl_int>(
					true));
			break;
	}

	switch(_hardness)
	{
		case splatter::hardness::hard:
			splat_float_->numerical_argument(
				"is_hard",
				static_cast<cl_int>(
					true));
			break;
		case splatter::hardness::soft:
			splat_float_->numerical_argument(
				"is_hard",
				static_cast<cl_int>(
					false));
			break;
	}

	switch(_mix_mode)
	{
		case splatter::mix_mode::add:
			splat_float_->numerical_argument(
				"do_add",
				static_cast<cl_int>(
					true));
			break;
		case splatter::mix_mode::set:
			splat_float_->numerical_argument(
				"do_add",
				static_cast<cl_int>(
					false));
			break;
	}


	splat_float_->enqueue_automatic(
		_rectangle.size().get());
}

void
flakelib::splatter::object::splat_planar_float2(
	planar::float_view const &,
	splatter::rectangle::object const &,
	cl::float2 const &,
	splatter::pen_type::type,
	splatter::hardness::type,
	splatter::mix_mode::type)
{
}

flakelib::splatter::object::~object()
{
}
