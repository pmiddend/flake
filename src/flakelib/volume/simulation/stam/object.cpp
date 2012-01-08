#include <flakelib/volume/simulation/stam/object.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/linear_view.hpp>
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/buffer_pool/volume_lock.hpp>
#include <flakelib/volume/laplace_solver/base.hpp>
#include <flakelib/profiler/scoped.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/image2d/view/size.hpp>
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
#include <fcppt/io/cout.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/basic_impl.hpp>

flakelib::volume::simulation::stam::object::object(
	sge::opencl::command_queue::object &_command_queue,
	volume::boundary::view const &_boundary,
	sge::parse::json::object const &_config_file,
	flakelib::build_options const &_build_options,
	buffer_pool::object &_buffer_pool,
	utility::object &_utility,
	volume::laplace_solver::base &_laplace_solver)
:
	command_queue_(
		_command_queue),
	utility_(
		_utility),
	buffer_pool_(
		_buffer_pool),
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
				FCPPT_TEXT("kernels/volume/stam.cl"))),
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
	boundary_(
		_boundary),
	// This has to be initialized here because we need it as the "initial
	// guess" for further computations. It will be zero-initialized in the
	// ctor.
	velocity_image_(
		fcppt::make_unique_ptr<volume_float4_lock>(
			fcppt::ref(
				buffer_pool_),
			boundary_.get().size())),
	// All of those images are initialized "along the way" in object::update
	vector_magnitude_image_(),
	residual_image_(),
	pressure_image_()
{
	// Initialize velocity
	utility_.null_buffer(
		buffer::linear_view<cl_float>(
			velocity_image_->value().buffer()));
}

// @override
flakelib::buffer::volume_view<cl_float4> const
flakelib::volume::simulation::stam::object::velocity()
{
	return velocity_image_->value();
}

flakelib::profiler::object const &
flakelib::volume::simulation::stam::object::parent_profiler() const
{
	return parent_profiler_;
}

void
flakelib::volume::simulation::stam::object::update(
	flakelib::duration const &dt)
{
	profiler::scoped scoped_profiler(
		parent_profiler_,
		command_queue_);

	this->apply_forces(
		velocity_image_->value());

	unique_volume_float4_lock advected =
		this->advect(
			velocity_image_->value(),
			dt);

	// The old version of the velocity is already advected (into the
	// "advected" variable), we don't need it anymore.
	velocity_image_.reset();

	unique_volume_float_lock divergence_result =
		this->divergence(
			advected->value());

	unique_volume_float_lock pressure_result =
		this->solve(
			divergence_result->value());

	velocity_image_ =
		this->gradient_and_subtract(
			stam::vector_field(
				advected->value()),
			stam::pressure(
				pressure_result->value()));
}

flakelib::volume::simulation::stam::object::~object()
{
}

flakelib::volume::simulation::stam::object::unique_volume_float4_lock
flakelib::volume::simulation::stam::object::advect(
	volume_float4_view const &from,
	flakelib::duration const &dt)
{
	profiler::scoped scoped_profiler(
		advection_profiler_,
		command_queue_);

	unique_volume_float4_lock result(
		fcppt::make_unique_ptr<volume_float4_lock>(
			fcppt::ref(
				buffer_pool_),
			from.size()));

	FCPPT_ASSERT_PRE(
		&(result->value().buffer()) != &(from.buffer()));

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
		boundary_.get().buffer());

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
			(from.size()[1])
			(from.size()[2]).container());

	return
		fcppt::move(
			result);
}

void
flakelib::volume::simulation::stam::object::apply_forces(
	volume_float4_view const &from)
{
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
			(from.size()[0])
			(from.size()[1]).container());
}

flakelib::volume::simulation::stam::object::unique_volume_float_lock
flakelib::volume::simulation::stam::object::divergence(
	volume_float4_view const &from)
{
	profiler::scoped scoped_profiler(
		divergence_profiler_,
		command_queue_);

	unique_volume_float_lock result(
		fcppt::make_unique_ptr<volume_float_lock>(
			fcppt::ref(
				buffer_pool_),
			from.size()));

	FCPPT_ASSERT_PRE(
		&(result->value().buffer()) != &(from.buffer()));

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
		boundary_.get().buffer());

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
			(from.size()[1])
			(from.size()[2]).container());

	return
		fcppt::move(
			result);
}

flakelib::volume::simulation::stam::object::unique_volume_float_lock
flakelib::volume::simulation::stam::object::solve(
	volume_float_view const &_rhs)
{
	profiler::scoped scoped_profiler(
		solve_profiler_,
		command_queue_);

	unique_volume_float_lock result(
		fcppt::make_unique_ptr<volume_float_lock>(
			fcppt::ref(
				buffer_pool_),
			_rhs.size()));

	unique_volume_float_lock initial_guess(
		fcppt::make_unique_ptr<volume_float_lock>(
			fcppt::ref(
				buffer_pool_),
			_rhs.size()));

	utility_.null_buffer(
		buffer::linear_view<cl_float>(
			initial_guess->value().buffer()));

	laplace_solver_.solve(
		volume::laplace_solver::rhs(
			_rhs),
		volume::laplace_solver::destination(
			result->value()),
		volume::laplace_solver::initial_guess(
			initial_guess->value()));

	return
		fcppt::move(
			result);
}

flakelib::volume::simulation::stam::object::unique_volume_float4_lock
flakelib::volume::simulation::stam::object::gradient_and_subtract(
	stam::vector_field const &_vector_field,
	stam::pressure const &_pressure)
{
	profiler::scoped scoped_profiler(
		project_profiler_,
		command_queue_);

	unique_volume_float4_lock result(
		fcppt::make_unique_ptr<volume_float4_lock>(
			fcppt::ref(
				buffer_pool_),
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
		boundary_.get().buffer());

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
			(_pressure.get().size()[1])
			(_pressure.get().size()[2]).container());

	return
		fcppt::move(
			result);
}
