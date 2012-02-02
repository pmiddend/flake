#include <flakelib/buffer_pool/planar_lock_impl.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assign/make_array.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/planar_view_impl.hpp>
#include <flakelib/planar/simulation/stam/vorticity.hpp>
#include <flakelib/planar/unique_float_buffer_lock.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/text.hpp>
#include <fcppt/chrono/duration_impl.hpp>

flakelib::planar::simulation::stam::vorticity::vorticity(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options,
	buffer_pool::object &_buffer_pool,
	planar::boundary_buffer_view const &_boundary,
	stam::vorticity_strength const &_vorticity_strength)
:
	command_queue_(
		_command_queue),
	buffer_pool_(
		_buffer_pool),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/planar/vorticity.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	vorticity_kernel_(
		program_,
		sge::opencl::kernel::name(
			"vorticity")),
	gradient_and_cross_kernel_(
		program_,
		sge::opencl::kernel::name(
			"gradient_and_cross"))
{
	gradient_and_cross_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_vorticity_strength.get());

	vorticity_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_boundary.get().buffer());
}

void
flakelib::planar::simulation::stam::vorticity::vorticity_strength(
	cl_float const _vorticity_strength)
{
	gradient_and_cross_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_vorticity_strength);
}

void
flakelib::planar::simulation::stam::vorticity::update(
	planar::float2_view const &_view,
	flakelib::duration const &_duration)
{
	planar::unique_float_buffer_lock vorticity_result(
		fcppt::make_unique_ptr<planar::float_buffer_lock>(
			fcppt::ref(
				buffer_pool_),
			_view.size()));

	vorticity_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_view.buffer());

	vorticity_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		vorticity_result->value().buffer());

	vorticity_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_int>(
			_view.size()[0]));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		vorticity_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_view.size()[0])
			(_view.size()[1]).container());

	gradient_and_cross_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		static_cast<cl_int>(
			_view.size()[0]));

	gradient_and_cross_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		vorticity_result->value().buffer());

	gradient_and_cross_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		_view.buffer());

	gradient_and_cross_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		static_cast<cl_float>(
			_duration.count()));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		gradient_and_cross_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_view.size()[0])
			(_view.size()[1]).container());
}

flakelib::planar::simulation::stam::vorticity::~vorticity()
{
}
