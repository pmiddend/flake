#include <flake/media_path_from_string.hpp>
#include <flake/volume/flakes/mover.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <fcppt/make_ref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/reference_to_base.hpp>
#include <fcppt/math/dim/output.hpp>


flake::volume::flakes::mover::mover(
	flakelib::cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool,
	flakes::position_view const &_positions,
	flakes::point_size_view const &_point_sizes,
	flakes::snow_density_view const &_snow_density,
	flakes::collision_increment const &_collision_increment,
	flakes::activity_view const &_activity,
	flakes::minimum_size const &_minimum_size,
	flakes::maximum_size const &_maximum_size,
	flakes::gravity_magnitude const &_gravity_magnitude,
	flakelib::marching_cubes::iso_level const &_iso_level)
:
	program_(
		_program_context.log_context(),
		_program_context.command_queue(),
		flake::media_path_from_string(
			FCPPT_TEXT("kernels/flake/flakes/mover.cl")),
		_program_context.compiler_flags()),
	initialize_velocities_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"initialize_velocities"))),
	move_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"move"))),
	update_activity_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"update_activity"))),
	positions_(
		_positions),
	point_sizes_(
		_point_sizes),
	activity_(
		_activity),
	velocities_(
		fcppt::make_unique_ptr<linear_float4_lock>(
			_buffer_pool,
			sge::opencl::dim1(
				_positions.get().size().w()))),
	time_(
		0.0f)
{
	this->initialize_velocities(
		_minimum_size,
		_maximum_size);

	update_activity_kernel_->numerical_argument(
		"iso_level",
		sge::opencl::kernel::numeric_type(
			_iso_level.get()));

	update_activity_kernel_->buffer_argument(
		"snow_density",
		_snow_density.get().buffer());

	update_activity_kernel_->numerical_argument(
		"buffer_pitch",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_snow_density.get().size().w())));

	update_activity_kernel_->buffer_argument(
		"activity",
		_activity.get().buffer());

	move_kernel_->numerical_argument(
		"minimum_size",
		sge::opencl::kernel::numeric_type(
			_minimum_size.get()));

	move_kernel_->buffer_argument(
		"velocities",
		velocities_->value().buffer());

	move_kernel_->numerical_argument(
		"maximum_size",
		sge::opencl::kernel::numeric_type(
			_maximum_size.get()));

	move_kernel_->numerical_argument(
		"gravity_magnitude",
		sge::opencl::kernel::numeric_type(
			_gravity_magnitude.get()));

	move_kernel_->buffer_argument(
		"activity",
		_activity.get().buffer());

	move_kernel_->buffer_argument(
		"sizes",
		_point_sizes.get().buffer());

	move_kernel_->buffer_argument(
		"positions",
		_positions.get().buffer());

	move_kernel_->buffer_argument(
		"snow_density",
		_snow_density.get().buffer());

	move_kernel_->numerical_argument(
		"collision_increment",
		sge::opencl::kernel::numeric_type(
			_collision_increment.get()));
}

void
flake::volume::flakes::mover::update(
	flakelib::duration const &_delta,
	flakelib::volume::velocity_buffer_view const &_velocity,
	flakelib::volume::boundary_buffer_view const &_boundary,
	flake::volume::flakes::count const &_count)
{
	time_ +=
		_delta.count();

	this->update_activity(
		_boundary);

	move_kernel_->numerical_argument(
		"time",
		sge::opencl::kernel::numeric_type(
			time_));

	move_kernel_->numerical_argument(
		"time_delta",
		sge::opencl::kernel::numeric_type(
			_delta.count()));

	move_kernel_->buffer_argument(
		"fluid_velocity",
		_velocity.get().buffer());

	move_kernel_->numerical_argument(
		"bounding_volume_width",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_int>(
				_boundary.get().size().w())));

	move_kernel_->numerical_argument(
		"bounding_volume_height",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_int>(
				_boundary.get().size().h())));

	move_kernel_->numerical_argument(
		"bounding_volume_depth",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_int>(
				_boundary.get().size().d())));

	move_kernel_->numerical_argument(
		"buffer_pitch",
		sge::opencl::kernel::numeric_type(
			static_cast<cl_uint>(
				_velocity.get().size().w())));

	sge::opencl::memory_object::scoped_objects scoped_vb{
		move_kernel_->command_queue(),
		sge::opencl::memory_object::base_ref_sequence{
			&positions_.get().buffer(),
			&point_sizes_.get().buffer()
		}
	};

	move_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim1(
			sge::opencl::dim1(
				_count.get())));
}

flake::volume::flakes::mover::~mover()
{
}

void
flake::volume::flakes::mover::initialize_velocities(
	flakes::minimum_size const &_minimum_size,
	flakes::maximum_size const &_maximum_size)
{
	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		fcppt::reference_to_base<
			sge::opencl::memory_object::base
		>(
			fcppt::make_ref(
				point_sizes_.get().buffer()
			)
		)
	);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		move_kernel_->command_queue(),
		mem_objects);

	initialize_velocities_kernel_->numerical_argument(
		"minimum_size",
		sge::opencl::kernel::numeric_type(
			_minimum_size.get()));

	initialize_velocities_kernel_->numerical_argument(
		"maximum_size",
		sge::opencl::kernel::numeric_type(
			_maximum_size.get()));

	initialize_velocities_kernel_->buffer_argument(
		"velocities",
		velocities_->value().buffer());

	initialize_velocities_kernel_->buffer_argument(
		"sizes",
		point_sizes_.get().buffer());

	initialize_velocities_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim1(
			velocities_->value().size()));
}

void
flake::volume::flakes::mover::update_activity(
	flakelib::volume::boundary_buffer_view const &_boundary)
{
	update_activity_kernel_->buffer_argument(
		"boundary",
		_boundary.get().buffer());

	update_activity_kernel_->enqueue_automatic(
		sge::opencl::command_queue::global_dim3(
			_boundary.get().size()));
}
