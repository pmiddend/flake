#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/buffer_pool/planar_lock.hpp>
#include <flakelib/utility/object.hpp>
#include <flakelib/volume/density/advector.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/chrono/duration.hpp>


flakelib::volume::density::advector::advector(
	sge::opencl::command_queue::object &_command_queue,
	volume::boundary::view const &_boundary,
	flakelib::build_options const &_build_options,
	buffer_pool::object &_buffer_pool,
	utility::object &_utility,
	density::grid_scale const &_grid_scale)
:
	command_queue_(
		_command_queue),
	buffer_pool_(
		_buffer_pool),
	grid_scale_(
		_grid_scale.get()),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/volume/density_advector.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	apply_sources_kernel_(
		program_,
		sge::opencl::kernel::name(
			"apply_sources")),
	advect_kernel_(
		program_,
		sge::opencl::kernel::name(
			"advect")),
	sources_(
		buffer_pool_,
		_boundary.get().size()),
	current_density_(
		fcppt::make_unique_ptr<buffer_pool::volume_lock<cl_float> >(
			fcppt::ref(
				buffer_pool_),
			_boundary.get().size()))
{
	_utility.null_buffer(
		buffer::linear_view<cl_float>(
			sources_.value().buffer()));

	_utility.null_buffer(
		buffer::linear_view<cl_float>(
			current_density_->value().buffer()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		_boundary.get().buffer());
}

void
flakelib::volume::density::advector::update(
	buffer::volume_view<cl_float4> const &_velocity,
	flakelib::duration const &_dt)
{
	apply_sources_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		sources_.value().buffer());

	apply_sources_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		current_density_->value().buffer());

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		apply_sources_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(sources_.value().size().content()).container());

	unique_volume_float_lock target_density(
		fcppt::make_unique_ptr<buffer_pool::volume_lock<cl_float> >(
			fcppt::ref(
				buffer_pool_),
			current_density_->value().size()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_velocity.buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		current_density_->value().buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		target_density->value().buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		static_cast<cl_int>(
			sources_.value().size()[0]));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			5),
		static_cast<cl_float>(
			_dt.count()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			6),
		grid_scale_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		advect_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(sources_.value().size()[0])
			(sources_.value().size()[1])
			(sources_.value().size()[2]).container());

	current_density_.swap(
		target_density);
}

flakelib::buffer::volume_view<cl_float> const
flakelib::volume::density::advector::get()
{
	return current_density_->value();
}

flakelib::volume::density::advector::~advector()
{
}
