#include <sge/opencl/memory_object/size_type.hpp>
#include <fcppt/assign/make_array.hpp>
#include <flakelib/planar/simulation/stam/wind_source.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <fcppt/text.hpp>

flakelib::planar::simulation::stam::wind_source::wind_source(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options,
	stam::external_force_magnitude const &_external_force_magnitude)
:
	command_queue_(
		_command_queue),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/planar/wind_source.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	kernel_(
		program_,
		sge::opencl::kernel::name(
			"apply"))
{
	this->external_force_magnitude(
		_external_force_magnitude);
}

void
flakelib::planar::simulation::stam::wind_source::external_force_magnitude(
	stam::external_force_magnitude const &_external_force_magnitude)
{
	kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_external_force_magnitude.get());
}

void
flakelib::planar::simulation::stam::wind_source::update(
	buffer::planar_view<cl_float2> const &_v)
{
	kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_v.buffer());

	kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_int>(
			_v.size()[0]));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_v.size()[0]).container());
}

flakelib::planar::simulation::stam::wind_source::~wind_source()
{
}
