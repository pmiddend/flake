#include <flakelib/simulation/stam/object.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/planar_image_view_to_float_buffer.hpp>
#include <flakelib/laplace_solver/base.hpp>
#include <flakelib/buffer_pool/planar_lock.hpp>
#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/profiler/scoped.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/opencl/program/source_string_sequence.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/optional.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/chrono/duration_impl.hpp>
#include <fcppt/container/array.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <fcppt/config/external_end.hpp>


flakelib::simulation::stam::object::object(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::boundary_view const &_boundary_image,
	sge::parse::json::object const &_config_file,
	flakelib::build_options const &_build_options,
	buffer_pool::object &_buffer_cache,
	utility::object &_utility,
	laplace_solver::base &_laplace_solver)
:
	command_queue_(
		_command_queue),
	utility_(
		_utility),
	buffer_cache_(
		_buffer_cache),
	laplace_solver_(
		_laplace_solver),
	external_force_magnitude_(
		sge::parse::json::find_and_convert_member<cl_float>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("external-force-magnitude")))),
	grid_scale_(
		sge::parse::json::find_and_convert_member<cl_float>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("grid-scale")))),
	profiling_enabled_(
		sge::parse::json::find_and_convert_member<bool>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("profiling")))),
	main_program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/stam.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	advect_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"advect")),
	apply_external_forces_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"apply_external_forces")),
	divergence_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"divergence")),
	laplacian_residual_absolute_value_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"laplacian_residual_absolute_value")),
	gradient_and_subtract_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"gradient_and_subtract")),
	parent_profiler_(
		FCPPT_TEXT("stam simulation"),
		profiler::optional_parent(),
		profiling_enabled_ ? profiler::activation::enabled : profiler::activation::disabled),
	advection_profiler_(
		FCPPT_TEXT("advection"),
		profiler::optional_parent(
			parent_profiler_),
		profiling_enabled_ ? profiler::activation::enabled : profiler::activation::disabled),
	external_forces_profiler_(
		FCPPT_TEXT("apply_external_forces"),
		profiler::optional_parent(
			parent_profiler_),
		profiling_enabled_ ? profiler::activation::enabled : profiler::activation::disabled),
	divergence_profiler_(
		FCPPT_TEXT("divergence"),
		profiler::optional_parent(
			parent_profiler_),
		profiling_enabled_ ? profiler::activation::enabled : profiler::activation::disabled),
	project_profiler_(
		FCPPT_TEXT("project"),
		profiler::optional_parent(
			parent_profiler_),
		profiling_enabled_ ? profiler::activation::enabled : profiler::activation::disabled),
	solve_profiler_(
		FCPPT_TEXT("solve"),
		profiler::optional_parent(
			parent_profiler_),
		profiling_enabled_ ? profiler::activation::enabled : profiler::activation::disabled),
	additional_planar_data_(),
	boundary_image_(
		buffer_cache_,
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				_boundary_image.get()))),
	// This has to be initialized here because we need it as the "initial
	// guess" for further computations. It will be zero-initialized in the
	// ctor.
	velocity_image_(
		fcppt::make_unique_ptr<planar_float2_lock>(
			fcppt::ref(
				buffer_cache_),
			boundary_image_.value().size())),
	// All of those images are initialized "along the way" in object::update
	divergence_image_(),
	vector_magnitude_image_(),
	residual_image_(),
	pressure_image_()
{
	// Initialize additional data. The names here are used in the configuration
	// file with the suffix "-scale" to determine how they should be scaled when
	// viewing them. As such, they shouldn't contain any whitespaces
	fcppt::container::array<fcppt::string,4> const additional_data =
		{{
			 FCPPT_TEXT("pressure"),
			 FCPPT_TEXT("velocity-magnitude"),
			 FCPPT_TEXT("divergence"),
			 FCPPT_TEXT("residual"),
		 }};

	for(std::size_t i = 0; i < additional_data.size(); ++i)
		additional_planar_data_.push_back(
			additional_planar_data::value_type(
				additional_data[i],
				// Leave the actual image empty for now
				0));

	// Initialize velocity
	utility_.null_buffer(
		buffer::linear_view<cl_float>(
			velocity_image_->value().buffer()));

	cl::planar_image_view_to_float_buffer(
		command_queue_,
		_boundary_image.get(),
		boundary_image_.value());
}

flakelib::buffer::planar_view<cl_float2> const
flakelib::simulation::stam::object::velocity()
{
	FCPPT_ASSERT_PRE(
		velocity_image_);

	return
		velocity_image_->value();
}

flakelib::additional_planar_data const &
flakelib::simulation::stam::object::additional_planar_data() const
{
	if(pressure_image_)
		additional_planar_data_[FCPPT_TEXT("pressure")] =
			&pressure_image_->value();

	if(divergence_image_)
		additional_planar_data_[FCPPT_TEXT("divergence")] =
			&divergence_image_->value();

	if(vector_magnitude_image_)
		additional_planar_data_[FCPPT_TEXT("velocity-magnitude")] =
			&vector_magnitude_image_->value();

	if(residual_image_)
		additional_planar_data_[FCPPT_TEXT("residual")] =
			&residual_image_->value();

	return
		additional_planar_data_;
}

flakelib::profiler::object const &
flakelib::simulation::stam::object::parent_profiler() const
{
	return parent_profiler_;
}

void
flakelib::simulation::stam::object::update(
	flakelib::duration const &dt)
{
	profiler::scoped scoped_profiler(
		parent_profiler_,
		command_queue_);

	this->apply_forces(
		velocity_image_->value());

	unique_planar_float2_lock advected =
		this->advect(
			velocity_image_->value(),
			dt);

	// The old version of the velocity is already advected (into the
	// "advected" variable), we don't need it anymore.
	velocity_image_.reset();

	// We need the divergence only in solve, so we could discard it here,
	// but we want to visualize it.
	divergence_image_ =
		this->divergence(
			advected->value());

	// Now, the pressure, we don't need later. We could discard it after
	// gradient_and_subtract, but we want to visualize it.
	pressure_image_ =
		this->solve(
			divergence_image_->value());

	velocity_image_ =
		this->gradient_and_subtract(
			stam::vector_field(
				advected->value()),
			stam::pressure(
				pressure_image_->value()));

	vector_magnitude_image_ =
		this->vector_magnitude(
			velocity_image_->value());

	residual_image_ =
		this->laplacian_residual(
			stam::solution(
				pressure_image_->value()),
			stam::rhs(
				divergence_image_->value()));
}

flakelib::simulation::stam::object::~object()
{
	if(profiling_enabled_)
		fcppt::io::cout() << parent_profiler_ << FCPPT_TEXT("\n");
}

flakelib::simulation::stam::object::unique_planar_float2_lock
flakelib::simulation::stam::object::advect(
	planar_float2_view const &from,
	flakelib::duration const &dt)
{
	profiler::scoped scoped_profiler(
		advection_profiler_,
		command_queue_);

	unique_planar_float2_lock result(
		fcppt::make_unique_ptr<planar_float2_lock>(
			fcppt::ref(
				buffer_cache_),
			from.size()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from.buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		result->value().buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		boundary_image_.value().buffer());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_int>(
			from.size()[0]));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		static_cast<cl_float>(
			dt.count()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			5),
		grid_scale_);


	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		advect_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(from.size()[0])
			(from.size()[1]).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(8)
			(8).container());

	return
		fcppt::move(
			result);
}

void
flakelib::simulation::stam::object::apply_forces(
	planar_float2_view const &from)
{
	FCPPT_ASSERT_PRE(
		from.size()[0] == from.size()[1]);

	profiler::scoped scoped_profiler(
		external_forces_profiler_,
		command_queue_);

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from.buffer());

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		external_force_magnitude_);

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_int>(
			from.size()[0]));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		apply_external_forces_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(from.size()[0]).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(64).container());
}

flakelib::simulation::stam::object::unique_planar_float_lock
flakelib::simulation::stam::object::divergence(
	planar_float2_view const &from)
{
	profiler::scoped scoped_profiler(
		divergence_profiler_,
		command_queue_);

	unique_planar_float_lock result(
		fcppt::make_unique_ptr<planar_float_lock>(
			fcppt::ref(
				buffer_cache_),
			from.size()));

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from.buffer());

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		result->value().buffer());

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		boundary_image_.value().buffer());

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_int>(
			from.size()[0]));

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		grid_scale_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		divergence_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(from.size()[0])
			(from.size()[1]).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(8)
			(8).container());

	return
		fcppt::move(
			result);
}

flakelib::simulation::stam::object::unique_planar_float_lock
flakelib::simulation::stam::object::solve(
	planar_float_view const &_rhs)
{
	profiler::scoped scoped_profiler(
		solve_profiler_,
		command_queue_);

	unique_planar_float_lock result(
		fcppt::make_unique_ptr<planar_float_lock>(
			fcppt::ref(
				buffer_cache_),
			_rhs.size()));

	unique_planar_float_lock initial_guess(
		fcppt::make_unique_ptr<planar_float_lock>(
			fcppt::ref(
				buffer_cache_),
			_rhs.size()));

	utility_.null_buffer(
		buffer::linear_view<cl_float>(
			initial_guess->value().buffer()));

	laplace_solver_.solve(
		laplace_solver::rhs(
			_rhs),
		laplace_solver::destination(
			result->value()),
		laplace_solver::initial_guess(
			initial_guess->value()),
		laplace_solver::boundary(
			boundary_image_.value()));

	return
		fcppt::move(
			result);
}

flakelib::simulation::stam::object::unique_planar_float2_lock
flakelib::simulation::stam::object::gradient_and_subtract(
	stam::vector_field const &_vector_field,
	stam::pressure const &_pressure)
{
	profiler::scoped scoped_profiler(
		project_profiler_,
		command_queue_);

	unique_planar_float2_lock result(
		fcppt::make_unique_ptr<planar_float2_lock>(
			fcppt::ref(
				buffer_cache_),
			_pressure.get().size()));

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_pressure.get().buffer());

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		grid_scale_);

	// Temporary vector field (from which the divergence was calculated)
	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_vector_field.get().buffer());

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		boundary_image_.value().buffer());

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		result->value().buffer());

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			5),
		static_cast<cl_int>(
			_vector_field.get().size()[0]));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		gradient_and_subtract_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_pressure.get().size()[0])
			(_pressure.get().size()[1]).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(8)
			(8).container());

	return
		fcppt::move(
			result);
}

flakelib::simulation::stam::object::unique_planar_float_lock
flakelib::simulation::stam::object::laplacian_residual(
	stam::solution const &_solution,
	stam::rhs const &_divergence)
{
	FCPPT_ASSERT_PRE(
		_solution.get().size() == _divergence.get().size());

	unique_planar_float_lock result(
		fcppt::make_unique_ptr<planar_float_lock>(
			fcppt::ref(
				buffer_cache_),
			_solution.get().size()));

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_solution.get().buffer());

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		boundary_image_.value().buffer());

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_divergence.get().buffer());

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		result->value().buffer());

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		grid_scale_);

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			5),
		static_cast<cl_int>(
			_divergence.get().size()[0]));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		laplacian_residual_absolute_value_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_solution.get().size()[0])
			(_solution.get().size()[1]).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(8)
			(8).container());

	return
		fcppt::move(
			result);
}

flakelib::simulation::stam::object::unique_planar_float_lock
flakelib::simulation::stam::object::vector_magnitude(
	planar_float2_view const &_from)
{
	unique_planar_float_lock result(
		fcppt::make_unique_ptr<planar_float_lock>(
			fcppt::ref(
				buffer_cache_),
			_from.size()));

	/*
	utility_.planar_vector_magnitude(
		utility::from(
			_from),
		utility::to(
			result->value()),
		utility::multiplier(
			1.0f));
			*/

	return
		fcppt::move(
			result);
}
