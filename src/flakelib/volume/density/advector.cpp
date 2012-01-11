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
	flakelib::duration const &)
{
}

buffer::volume_view<cl_float> const
flakelib::volume::density::advector::get()
{
}

flakelib::volume::density::advector::~advector()
{
}
#endif
