#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/simulation/stam/buissnesq/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::volume::simulation::stam::buissnesq::object::object(
	cl::program_context const &_program_context,
	buissnesq::density_strength const &_density_strength,
	buissnesq::temperature_strength const &_temperature_strength,
	buissnesq::ambient_temperature const &_ambient_temperature)
:
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/volume/buissnesq.cl")),
		_program_context.compiler_flags()),
	kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"buissnesq_apply")))
{
	kernel_->numerical_argument(
		"density_strength",
		_density_strength.get());

	kernel_->numerical_argument(
		"temperature_strength",
		_temperature_strength.get());

	kernel_->numerical_argument(
		"ambient_temperature",
		_ambient_temperature.get());
}

void
flakelib::volume::simulation::stam::buissnesq::object::update(
	stam::velocity const &_velocity,
	buissnesq::density_view const &_density,
	buissnesq::temperature_view const &_temperature,
	flakelib::duration const &_delta)
{
	FCPPT_ASSERT_PRE(
		_velocity.get().size() == _density.get().size());

	FCPPT_ASSERT_PRE(
		_velocity.get().size() == _temperature.get().size());

	kernel_->buffer_argument(
		"velocity",
		_velocity.get().buffer());

	kernel_->buffer_argument(
		"density",
		_density.get().buffer());

	kernel_->buffer_argument(
		"temperature",
		_temperature.get().buffer());

	kernel_->numerical_argument(
		"time_delta",
		_delta.count());

	kernel_->enqueue_automatic(
		flakelib::cl::global_dim1(
			sge::opencl::memory_object::dim1(
				_velocity.get().size().content())));
}

flakelib::volume::simulation::stam::buissnesq::object::~object()
{
}
