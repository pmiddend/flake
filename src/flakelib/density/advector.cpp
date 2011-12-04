#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/density/advector.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/chrono/duration.hpp>
#include <fcppt/math/dim/basic_impl.hpp>


flakelib::density::advector::advector(
	sge::opencl::command_queue::object &_command_queue,
	planar_pool::object &_image_pool,
	utility::object &_utility,
	density::grid_dimensions const &_grid_dimensions,
	density::grid_scale const &_grid_scale)
:
	command_queue_(
		_command_queue),
	image_pool_(
		_image_pool),
	grid_scale_(
		_grid_scale.get()),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/density_advector.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters())),
	apply_sources_kernel_(
		program_,
		sge::opencl::kernel::name(
			"apply_sources")),
	advect_kernel_(
		program_,
		sge::opencl::kernel::name(
			"advect")),
	sources_(
		image_pool_,
		_grid_dimensions.get()),
	current_density_(
		fcppt::make_unique_ptr<planar_pool::scoped_lock>(
			fcppt::ref(
				image_pool_),
			_grid_dimensions.get()))
{
	_utility.null_image(
		sources_.value());

	_utility.null_image(
		current_density_->value());
}

void
flakelib::density::advector::update(
	sge::opencl::memory_object::image::planar &_velocity,
	flakelib::duration const &_dt)
{
	apply_sources_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		sources_.value());

	apply_sources_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		current_density_->value());

	flakelib::cl::apply_kernel_to_planar_image(
		apply_sources_kernel_,
		command_queue_,
		current_density_->value());

	flakelib::planar_pool::unique_lock target_density(
		fcppt::make_unique_ptr<planar_pool::scoped_lock>(
			fcppt::ref(
				image_pool_),
			current_density_->value().size()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		current_density_->value());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		target_density->value());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_velocity);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		grid_scale_);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		static_cast<cl_float>(
			_dt.count()));

	flakelib::cl::apply_kernel_to_planar_image(
		advect_kernel_,
		command_queue_,
		current_density_->value());

	current_density_.swap(
		target_density);
}

sge::opencl::memory_object::image::planar &
flakelib::density::advector::density_image()
{
	return current_density_->value();
}

sge::opencl::memory_object::image::planar &
flakelib::density::advector::source_image()
{
	return sources_.value();
}

flakelib::density::advector::~advector()
{
}
