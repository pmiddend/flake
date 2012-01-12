#if 0
#include <flakelib/volume/density/advector.hpp>

flakelib::volume::density::advector::advector(
	sge::opencl::command_queue::object &_command_queue,
	volume::boundary::view const &_boundary,
	sge::parse::json::object const &_config_file,
	flakelib::build_options const &_build_options,
	buffer_pool::object &_buffer_pool,
	utility::object &_utility)
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

	unique_planar_float_lock target_density(
		fcppt::make_unique_ptr<buffer_pool::planar_lock<cl_float> >(
			fcppt::ref(
				buffer_pool_),
			current_density_->value().size()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		current_density_->value().buffer());

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

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			5),
		grid_scale_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		advect_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(sources_.value().size()[0])
			(sources_.value().size()[1])
			(sources_.value().size()[1]).container());

	current_density_.swap(
		target_density);
}

buffer::volume_view<cl_float> const
flakelib::volume::density::advector::get()
{
	return current_density_->value();
}

flakelib::volume::density::advector::~advector()
{
}
#endif
