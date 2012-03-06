#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/splatter/object.hpp>
#include <flakelib/splatter/circle/object.hpp>
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
	splat_float_circle_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"splat_float_circle"))),
	splat_float2_circle_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"splat_float2_circle"))),
	splat_float_rectangle_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"splat_float_rectangle"))),
	splat_float2_rectangle_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"splat_float2_rectangle")))
{
}

void
flakelib::splatter::object::splat_float_circle(
	planar::float_view const &_buffer,
	splatter::circle::object const &_circle,
	cl_float const _value)
{
	splat_float_circle_->buffer_argument(
		"input",
		_buffer.buffer());

	splat_float_circle_->numerical_argument(
		"circle_position_x",
		_circle.position().get().w());

	splat_float_circle_->numerical_argument(
		"circle_position_y",
		_circle.position().get().h());

	splat_float_circle_->numerical_argument(
		"circle_radius",
		_circle.radius().get());

	splat_float_circle_->numerical_argument(
		"value",
		_circle.radius().get());

	splat_float_circle_->enqueue_automatic(
		_buffer.size());
}

void
flakelib::splatter::object::splat_float2_circle(
	planar::float2_view const &,
	splatter::circle::object const &,
	cl::float2 const &)
{
}

void
flakelib::splatter::object::splat_float_rectangle(
	planar::float_view const &,
	splatter::rectangle::object const &,
	cl_float)
{
}

void
flakelib::splatter::object::splat_float2_rectangle(
	planar::float2_view const &,
	splatter::rectangle::object const &,
	cl::float2 const &)
{
}

flakelib::splatter::object::~object()
{
}
