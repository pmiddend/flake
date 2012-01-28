#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/planar/density/advector.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/chrono/duration.hpp>
#include <fcppt/math/dim/basic_impl.hpp>


flakelib::planar::density::advector::advector(
	sge::opencl::command_queue::object &_command_queue,
	buffer_pool::object &_buffer_pool,
	utility::object &_utility,
	flakelib::build_options const &_build_options,
	density::grid_dimensions const &_grid_dimensions)
:
	command_queue_(
		_command_queue),
	buffer_pool_(
		_buffer_pool),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/planar/density_advector.cl"))),
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
		_grid_dimensions.get()),
	current_density_(
		fcppt::make_unique_ptr<buffer_pool::planar_lock<cl_float> >(
			fcppt::ref(
				buffer_pool_),
			_grid_dimensions.get()))
{
	_utility.null_buffer(
		buffer::linear_view<cl_float>(
			sources_.value().buffer()));

	_utility.null_buffer(
		buffer::linear_view<cl_float>(
			current_density_->value().buffer()));
}

void
flakelib::planar::density::advector::update(
	density::velocity_image const &_velocity,
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

	current_density_ =
		this->advect(
			current_density_->value(),
			_velocity,
			_dt);
}

flakelib::buffer::planar_view<cl_float> const
flakelib::planar::density::advector::density_image()
{
	return current_density_->value();
}

flakelib::buffer::planar_view<cl_float> const
flakelib::planar::density::advector::source_image()
{
	return sources_.value();
}

flakelib::planar::density::advector::~advector()
{
}

flakelib::planar::density::advector::unique_planar_float_lock
flakelib::planar::density::advector::advect(
	buffer::planar_view<cl_float> const &v,
	density::velocity_image const &_velocity,
	flakelib::duration const &_dt)
{
	unique_planar_float_lock target_density(
		fcppt::make_unique_ptr<buffer_pool::planar_lock<cl_float> >(
			fcppt::ref(
				buffer_pool_),
			current_density_->value().size()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		v.buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		target_density->value().buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_velocity.get().buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_int>(
			_velocity.get().size()[0]));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		static_cast<cl_float>(
			_dt.count()));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		advect_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(sources_.value().size()[0])
			(sources_.value().size()[1]).container());

	return
		fcppt::move(
			target_density);
}
