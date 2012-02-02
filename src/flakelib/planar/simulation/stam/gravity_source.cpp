#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/planar/simulation/stam/gravity_source.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/chrono/duration_impl.hpp>


flakelib::planar::simulation::stam::gravity_source::gravity_source(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options,
	stam::gravity_magnitude const &_gravity_magnitude)
:
	command_queue_(
		_command_queue),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/planar/gravity_source.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	kernel_(
		program_,
		sge::opencl::kernel::name(
			"apply"))
{
	this->gravity_magnitude(
		_gravity_magnitude);
}

void
flakelib::planar::simulation::stam::gravity_source::gravity_magnitude(
	stam::gravity_magnitude const &_gravity_magnitude)
{
	kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_gravity_magnitude.get());
}

void
flakelib::planar::simulation::stam::gravity_source::update(
	buffer::planar_view<cl_float2> const &_v,
	flakelib::duration const &dt)
{
	kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_v.buffer());

	kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_float>(
			dt.count()));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_v.size().content()).container());
}

flakelib::planar::simulation::stam::gravity_source::~gravity_source()
{
}
