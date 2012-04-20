#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/simulation/stam/wind_source.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/text.hpp>
#include <fcppt/math/dim/is_quadratic.hpp>


flakelib::volume::simulation::stam::wind_source::wind_source(
	cl::program_context const &_program_context,
	stam::wind_strength const &_wind_strength)
:
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/volume/wind_source.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"wind_source_apply")))
{
	this->wind_strength(
		_wind_strength.get());
}

void
flakelib::volume::simulation::stam::wind_source::wind_strength(
	cl_float const _wind_strength)
{
	kernel_->numerical_argument(
		"wind_strength",
		_wind_strength);
}

void
flakelib::volume::simulation::stam::wind_source::update(
	volume::float4_view const &_view)
{
	FCPPT_ASSERT_PRE(
		fcppt::math::dim::is_quadratic(
			_view.size()));

	kernel_->buffer_argument(
		"input",
		_view.buffer());

	kernel_->numerical_argument(
		"line_pitch",
		static_cast<cl_uint>(
			_view.size().w()));

	kernel_->numerical_argument(
		"buffer_width",
		static_cast<cl_int>(
			_view.size().w()));

	kernel_->enqueue_automatic(
		sge::opencl::memory_object::dim2(
			_view.size().w(),
			_view.size().w()));
}

flakelib::volume::simulation::stam::wind_source::~wind_source()
{
}
