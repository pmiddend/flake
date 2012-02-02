#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/planar/buoyancy/object.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/chrono/duration_impl.hpp>


flakelib::planar::buoyancy::object::object(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options,
	//buoyancy::boundary const &_boundary,
	buoyancy::temperature_strength const &_temperature_strength,
	buoyancy::density_strength const &_density_strength,
	buoyancy::ambient_temperature const &_ambient_temperature)
:
	command_queue_(
		_command_queue),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/planar/buoyancy.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	kernel_(
		program_,
		sge::opencl::kernel::name(
			"apply")),
	temperature_strength_(
		_temperature_strength.get()),
	density_strength_(
		_density_strength.get()),
	ambient_temperature_(
		_ambient_temperature.get())
{
	/*
	kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_boundary.get().buffer());
		*/
}

void
flakelib::planar::buoyancy::object::temperature_strength(
	cl_float const _temperature_strength)
{
	temperature_strength_ =
		_temperature_strength;
}

void
flakelib::planar::buoyancy::object::density_strength(
	cl_float const _density_strength)
{
	density_strength_ =
		_density_strength;
}

void
flakelib::planar::buoyancy::object::ambient_temperature(
	cl_float const _ambient_temperature)
{
	ambient_temperature_ =
		_ambient_temperature;
}

void
flakelib::planar::buoyancy::object::update(
	buoyancy::velocity const &_velocity,
	buoyancy::temperature const &_temperature,
	buoyancy::density const &_density,
	flakelib::duration const &_dt)
{
	kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		temperature_strength_);

	kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		density_strength_);

	kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		ambient_temperature_);

	kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_float>(
			_dt.count()));

	kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		_velocity.get().buffer());

	kernel_.argument(
		sge::opencl::kernel::argument_index(
			5),
		_temperature.get().buffer());

	kernel_.argument(
		sge::opencl::kernel::argument_index(
			6),
		_density.get().buffer());

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_velocity.get().size().content()).container());
}

flakelib::planar::buoyancy::object::~object()
{
}
