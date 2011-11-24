#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/laplace_solver/base.hpp>
#include <flakelib/planar_pool/scoped_lock.hpp>
#include <flakelib/profiler/scoped.hpp>
#include <flakelib/simulation/stam/object.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/image/algorithm/may_overlap.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/command_queue/scoped_planar_mapping.hpp>
#include <sge/opencl/memory_object/create_image_format.hpp>
#include <sge/opencl/memory_object/rect.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
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
#include <iostream>
#include <fcppt/config/external_end.hpp>


flakelib::simulation::stam::object::object(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::boundary_view const &_boundary_image,
	sge::parse::json::object const &_config_file,
	simulation::arrow_image_cache const &_arrow_image_cache,
	simulation::scalar_image_cache const &_scalar_image_cache,
	utility::object &_utility,
	laplace_solver::base &_laplace_solver)
:
	command_queue_(
		_command_queue),
	utility_(
		_utility),
	arrow_image_cache_(
		_arrow_image_cache.get()),
	scalar_image_cache_(
		_scalar_image_cache.get()),
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
	velocity_magnitude_scale_(
		sge::parse::json::find_and_convert_member<cl_float>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("visualization/velocity-magnitude-scale")))),
	profiling_enabled_(
		sge::parse::json::find_and_convert_member<bool>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("profiling")))),
	debug_output_(
		sge::parse::json::find_and_convert_member<bool>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("debug-output")))),
	main_program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/stam.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters())),
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
	additional_planar_data_(),
	boundary_image_(
		fcppt::make_unique_ptr<planar_pool::scoped_lock>(
			fcppt::ref(
				scalar_image_cache_),
			fcppt::math::dim::structure_cast<sge::opencl::memory_object::rect::dim>(
				sge::image2d::view::size(
					_boundary_image.get())))),
	// This has to be initialized here because we need it as the "initial
	// guess" for further computations. It will be zero-initialized in the
	// ctor.
	velocity_image_(
		fcppt::make_unique_ptr<planar_pool::scoped_lock>(
			fcppt::ref(
				scalar_image_cache_),
			fcppt::math::dim::structure_cast<sge::opencl::memory_object::rect::dim>(
				sge::image2d::view::size(
					_boundary_image.get())))),
	// All of those images are initialized "along the way" in object::update
	divergence_image_(),
	vector_magnitude_image_(),
	residual_image_(),
	pressure_image_()
{
	// Initialize additional data
	fcppt::container::array<fcppt::string,4> const additional_data =
		{{
			 FCPPT_TEXT("pressure"),
			 FCPPT_TEXT("velocity magnitude"),
			 FCPPT_TEXT("divergence"),
			 FCPPT_TEXT("residual"),
		 }};

	for(std::size_t i = 0; i < additional_data.size(); ++i)
		additional_planar_data_.push_back(
			additional_planar_data::value_type(
				additional_data[i],
				// Leave the actual image empty for now
				flakelib::planar_object()));

	// Initialize velocity
	utility_.null_image(
		velocity_image_->value());

	// Initialize boundary
	sge::opencl::command_queue::scoped_planar_mapping scoped_image(
		_command_queue,
		boundary_image_->value(),
		CL_MAP_WRITE,
		sge::opencl::memory_object::rect(
			sge::opencl::memory_object::rect::vector::null(),
			boundary_image_->value().size()));

	sge::image2d::algorithm::copy_and_convert(
		_boundary_image.get(),
		scoped_image.view(),
		sge::image::algorithm::may_overlap::no);
}

flakelib::planar_object const
flakelib::simulation::stam::object::velocity()
{
	FCPPT_ASSERT_PRE(
		velocity_image_);

	return
		flakelib::planar_object(
			&velocity_image_->value());
}

flakelib::additional_planar_data const &
flakelib::simulation::stam::object::additional_planar_data() const
{
	if(pressure_image_)
		additional_planar_data_[FCPPT_TEXT("pressure")] =
			flakelib::planar_object(
				&pressure_image_->value());

	if(divergence_image_)
		additional_planar_data_[FCPPT_TEXT("divergence")] =
			flakelib::planar_object(
				&divergence_image_->value());

	if(vector_magnitude_image_)
		additional_planar_data_[FCPPT_TEXT("vector-magnitude")] =
			flakelib::planar_object(
				&vector_magnitude_image_->value());

	if(residual_image_)
		additional_planar_data_[FCPPT_TEXT("residual")] =
			flakelib::planar_object(
				&residual_image_->value());

	return
		additional_planar_data_;
}

void
flakelib::simulation::stam::object::update(
	flakelib::duration const &dt)
{
	profiler::scoped scoped_profiler(
		parent_profiler_,
		command_queue_);

	planar_pool::unique_lock advected =
		this->advect(
			this->apply_forces(
				velocity_image_->value(),
				dt)->value(),
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
				velocity_image_->value()),
			stam::rhs(
				divergence_image_->value()));
}

flakelib::simulation::stam::object::~object()
{
	if(profiling_enabled_)
		fcppt::io::cout() << parent_profiler_ << FCPPT_TEXT("\n");
}

flakelib::planar_pool::unique_lock
flakelib::simulation::stam::object::advect(
	sge::opencl::memory_object::image::planar &from,
	flakelib::duration const &dt)
{
	profiler::scoped scoped_profiler(
		advection_profiler_,
		command_queue_);

	flakelib::planar_pool::unique_lock result(
		fcppt::make_unique_ptr<flakelib::planar_pool::scoped_lock>(
			fcppt::ref(
				arrow_image_cache_),
			from.size()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		result->value());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		boundary_image_->value());

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_float>(
			dt.count()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		grid_scale_);

	flakelib::cl::apply_kernel_to_planar_image(
		advect_kernel_,
		command_queue_,
		result->value());

	return
		fcppt::move(
			result);
}

flakelib::planar_pool::unique_lock
flakelib::simulation::stam::object::apply_forces(
	sge::opencl::memory_object::image::planar &from,
	flakelib::duration const &dt)
{
	FCPPT_ASSERT_PRE(
		from.size()[0] == from.size()[1]);

	profiler::scoped scoped_profiler(
		external_forces_profiler_,
		command_queue_);

	flakelib::planar_pool::unique_lock result(
		fcppt::make_unique_ptr<flakelib::planar_pool::scoped_lock>(
			fcppt::ref(
				arrow_image_cache_),
			from.size()));

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		result->value());

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		external_force_magnitude_);

	cl_uint const fan_width = 5;

	// start
	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_uint>(
			fan_width));

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		grid_scale_);

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			5),
		dt.count());

	flakelib::cl::apply_kernel_to_planar_image(
		apply_external_forces_kernel_,
		command_queue_,
		result->value());

	return
		fcppt::move(
			result);
}

flakelib::planar_pool::unique_lock
flakelib::simulation::stam::object::divergence(
	sge::opencl::memory_object::image::planar &from)
{
	profiler::scoped scoped_profiler(
		divergence_profiler_,
		command_queue_);

	flakelib::planar_pool::unique_lock result(
		fcppt::make_unique_ptr<flakelib::planar_pool::scoped_lock>(
			fcppt::ref(
				scalar_image_cache_),
			from.size()));

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		result->value());

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		boundary_image_->value());

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		grid_scale_);

	flakelib::cl::apply_kernel_to_planar_image(
		divergence_kernel_,
		command_queue_,
		result->value());

	return
		fcppt::move(
			result);
}

flakelib::planar_pool::unique_lock
flakelib::simulation::stam::object::solve(
	sge::opencl::memory_object::image::planar &_rhs)
{
	flakelib::planar_pool::unique_lock result(
		fcppt::make_unique_ptr<flakelib::planar_pool::scoped_lock>(
			fcppt::ref(
				scalar_image_cache_),
			_rhs.size()));

	flakelib::planar_pool::unique_lock initial_guess(
		fcppt::make_unique_ptr<flakelib::planar_pool::scoped_lock>(
			fcppt::ref(
				scalar_image_cache_),
			_rhs.size()));

	utility_.null_image(
		initial_guess->value());

	laplace_solver_.solve(
		laplace_solver::rhs(
			_rhs),
		laplace_solver::destination(
			result->value()),
		laplace_solver::initial_guess(
			initial_guess->value()),
		laplace_solver::boundary(
			boundary_image_->value()));

	return
		fcppt::move(
			result);
}

flakelib::planar_pool::unique_lock
flakelib::simulation::stam::object::gradient_and_subtract(
	stam::vector_field const &_vector_field,
	stam::pressure const &_pressure)
{
	profiler::scoped scoped_profiler(
		project_profiler_,
		command_queue_);

	flakelib::planar_pool::unique_lock result(
		fcppt::make_unique_ptr<flakelib::planar_pool::scoped_lock>(
			fcppt::ref(
				arrow_image_cache_),
			_pressure.get().size()));

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_pressure.get());

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		grid_scale_);

	// Temporary vector field (from which the divergence was calculated)
	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_vector_field.get());

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		boundary_image_->value());

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		result->value());

	flakelib::cl::apply_kernel_to_planar_image(
		gradient_and_subtract_kernel_,
		command_queue_,
		result->value());

	return
		fcppt::move(
			result);
}

flakelib::planar_pool::unique_lock
flakelib::simulation::stam::object::laplacian_residual(
	stam::solution const &_solution,
	stam::rhs const &_divergence)
{
	FCPPT_ASSERT_PRE(
		_solution.get().size() == _divergence.get().size());

	flakelib::planar_pool::unique_lock result(
		fcppt::make_unique_ptr<flakelib::planar_pool::scoped_lock>(
			fcppt::ref(
				scalar_image_cache_),
			_solution.get().size()));

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_solution.get());

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_divergence.get());

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		result->value());

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		boundary_image_->value());

	laplacian_residual_absolute_value_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		grid_scale_);

	flakelib::cl::apply_kernel_to_planar_image(
		laplacian_residual_absolute_value_kernel_,
		command_queue_,
		result->value());

	return
		fcppt::move(
			result);
}

flakelib::planar_pool::unique_lock
flakelib::simulation::stam::object::vector_magnitude(
	sge::opencl::memory_object::image::planar &from)
{
	flakelib::planar_pool::unique_lock result(
		fcppt::make_unique_ptr<flakelib::planar_pool::scoped_lock>(
			fcppt::ref(
				scalar_image_cache_),
			from.size()));

	utility_.planar_vector_magnitude(
		utility::from(
			from),
		utility::to(
			result->value()),
		utility::multiplier(
			1.0f));

	return
		fcppt::move(
			result);
}
