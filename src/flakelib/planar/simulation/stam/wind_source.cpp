#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/planar/simulation/stam/wind_source.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/text.hpp>

flakelib::planar::simulation::stam::wind_source::wind_source(
	cl::program_context const &_program_context,
	stam::wind_strength const &_wind_strength)
:
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/planar/wind_source.cl")),
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
flakelib::planar::simulation::stam::wind_source::wind_strength(
	cl_float const _wind_strength)
{
	kernel_->numerical_argument(
		"wind_strength",
		_wind_strength);
}

void
flakelib::planar::simulation::stam::wind_source::update(
	planar::float2_view const &_view)
{
	kernel_->buffer_argument(
		"input",
		_view.buffer());

	kernel_->numerical_argument(
		"buffer_pitch",
		static_cast<cl_uint>(
			_view.size().w()));

	kernel_->enqueue_automatic(
		flakelib::cl::global_dim1(
			sge::opencl::memory_object::dim1(
				_view.size().h())));
}

flakelib::planar::simulation::stam::wind_source::~wind_source()
{
}
