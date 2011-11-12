// Algorithm outline:
//
// - Input: vector field 'v1' from the last iteration
// - Take the boundary texture; set every texel in 'v1' to the
//   zero vector (inplace)                                       [x]
//
// - Apply advection routine, return a new vector field 'v2'     [x]
//
// - (Apply diffusion)                                           [/]
//
// - Apply external forces. For example, set each vector on
//   the right side of the vector field to a specified value
//   (inplace)                                                   [x]
//
// - Compute divergence of 'v2', store in 'v1'                   [x]
//
// - Nullify 'p1' texture                                        [x]
//
// - Use Jacobi to compute the pressure 'p' from 'v1', iterate,
//   alternate between p1 and p2 for storage.                    [x]
//
// - Calculate gradient of 'p', subtract from 'v1', return 'v2'  [x]

#include <flakelib/media_path_from_string.hpp>
#include <flakelib/profiler/scoped.hpp>
#include <flakelib/simulation/stam.hpp>
#include <sge/image/algorithm/may_overlap.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/scoped_planar_mapping.hpp>
#include <sge/opencl/memory_object/create_image_format.hpp>
#include <sge/opencl/memory_object/rect.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/opencl/program/source_string_sequence.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <fcppt/optional.hpp>
#include <fcppt/text.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/chrono/duration_impl.hpp>
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


flakelib::simulation::stam::stam(
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_command_queue,
	flakelib::boundary_view const &boundary,
	sge::parse::json::object const &_config_file)
:
	command_queue_(
		_command_queue),
	v1_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_RG,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	v2_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_RG,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	divergence_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_RG,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	p1_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	p2_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	vector_magnitude_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	residual_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	boundary_(
		_context,
		sge::opencl::memory_object::flags_field(sge::opencl::memory_object::flags::read) | sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::create_image_format(
			CL_INTENSITY,
			CL_UNORM_INT8),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	main_program_(
		_context,
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/stam.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters())),
	null_image_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"null_image")),
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
	jacobi_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"jacobi")),
	laplace_residual_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"laplace_residual")),
	gradient_and_subtract_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"gradient_and_subtract")),
	copy_image_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"copy_image")),
	vector_magnitude_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"vector_magnitude")),
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
	jacobi_iterations_(
		sge::parse::json::find_and_convert_member<unsigned>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("jacobi-iterations")))),
	profiling_enabled_(
		sge::parse::json::find_and_convert_member<bool>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("profiling")))),
	parent_profiler_(
		FCPPT_TEXT("stam simulation"),
		profiler::optional_parent(),
		profiling_enabled_ ? profiler::activation::enabled : profiler::activation::disabled),
	null_image_profiler_(
		FCPPT_TEXT("null_image"),
		profiler::optional_parent(
			parent_profiler_),
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
	jacobi_profiler_(
		FCPPT_TEXT("jacobi"),
		profiler::optional_parent(
			parent_profiler_),
		profiling_enabled_ ? profiler::activation::enabled : profiler::activation::disabled),
	project_profiler_(
		FCPPT_TEXT("project"),
		profiler::optional_parent(
			parent_profiler_),
		profiling_enabled_ ? profiler::activation::enabled : profiler::activation::disabled),
	additional_planar_data_()
{
	additional_planar_data_.push_back(
		std::make_pair(
			fcppt::string(
				FCPPT_TEXT("pressure")),
			flakelib::planar_object(&p1_)));

	additional_planar_data_.push_back(
		std::make_pair(
			fcppt::string(
				FCPPT_TEXT("velocity-magnitude")),
			flakelib::planar_object(&vector_magnitude_)));

	additional_planar_data_.push_back(
		std::make_pair(
			fcppt::string(
				FCPPT_TEXT("divergence")),
			flakelib::planar_object(&divergence_)));

	additional_planar_data_.push_back(
		std::make_pair(
			fcppt::string(
				FCPPT_TEXT("residual")),
			flakelib::planar_object(&residual_)));

	this->null_image(
		v1_);

	this->null_image(
		v2_);

	{
		sge::image2d::dim const boundary_dim =
			sge::image2d::view::size(
				boundary.get());

		sge::opencl::command_queue::scoped_planar_mapping scoped_image(
			_command_queue,
			boundary_,
			CL_MAP_WRITE,
			sge::opencl::memory_object::rect(
				sge::opencl::memory_object::rect::vector::null(),
				fcppt::math::dim::structure_cast<sge::opencl::memory_object::rect::dim>(
					boundary_dim)));

		sge::image2d::algorithm::copy_and_convert(
			boundary.get(),
			scoped_image.view(),
			sge::image::algorithm::may_overlap::no);
	}
}

flakelib::planar_object const
flakelib::simulation::stam::velocity()
{
	return
		flakelib::planar_object(
			&v2_);
}

flakelib::additional_planar_data const &
flakelib::simulation::stam::additional_planar_data() const
{
	return
		additional_planar_data_;
}

void
flakelib::simulation::stam::update(
	flakelib::duration const &dt)
{
	profiler::scoped scoped_profiler(
		parent_profiler_,
		command_queue_);

	// Also a form of Dirichlet: Set the boundary at certain locations to be
	// inflow.
	this->apply_forces(
		v1_,
		v2_,
		dt);

	// NOTE: We should only run advect on a divergence-free vector field (aside
	// from boundary conditions?)
	// Advect from v2 to v1
	this->advect(
		dt,
		v2_,
		v1_);

	// Calculate divergence of v1 to v2
	this->divergence(
		v1_,
		divergence_);

	// Project v1, store result in v2
	this->project(
		v1_,
		divergence_,
		v2_);

	// Debug
	this->vector_magnitude(
		v2_,
		vector_magnitude_,
		velocity_magnitude_scale_);

	this->laplace_residual(
		p1_,
		divergence_,
		residual_);
}

flakelib::simulation::stam::~stam()
{
	fcppt::io::cout() << parent_profiler_ << FCPPT_TEXT("\n");
}

void
flakelib::simulation::stam::null_image(
	sge::opencl::memory_object::image::planar &input)
{
	profiler::scoped scoped_profiler(
		null_image_profiler_,
		command_queue_);

	null_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		input);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		null_image_kernel_,
		fcppt::assign::make_array<std::size_t>
			(input.size()[0])
			(input.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flakelib::simulation::stam::advect(
	flakelib::duration const &dt,
	sge::opencl::memory_object::image::planar &from,
	sge::opencl::memory_object::image::planar &to)
{
	profiler::scoped scoped_profiler(
		advection_profiler_,
		command_queue_);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		to);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		boundary_);

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_float>(
			dt.count()));

	advect_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		grid_scale_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		advect_kernel_,
		fcppt::assign::make_array<std::size_t>
			(from.size()[0])
			(from.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flakelib::simulation::stam::apply_forces(
	sge::opencl::memory_object::image::planar &input,
	sge::opencl::memory_object::image::planar &output,
	flakelib::duration const &dt)
{
	FCPPT_ASSERT_PRE(
		input.size()[0] == input.size()[1]);

	profiler::scoped scoped_profiler(
		external_forces_profiler_,
		command_queue_);

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		input);

	apply_external_forces_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		output);

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

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		apply_external_forces_kernel_,
		fcppt::assign::make_array<std::size_t>(input.size()[0]).container(),
		fcppt::assign::make_array<std::size_t>(1).container());

	//force_applied = true;
}

void
flakelib::simulation::stam::divergence(
	sge::opencl::memory_object::image::planar &from,
	sge::opencl::memory_object::image::planar &to)
{
	profiler::scoped scoped_profiler(
		divergence_profiler_,
		command_queue_);

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		to);

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		boundary_);

	divergence_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		grid_scale_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		divergence_kernel_,
		fcppt::assign::make_array<std::size_t>
			(from.size()[0])
			(from.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flakelib::simulation::stam::project(
	sge::opencl::memory_object::image::planar &_velocity,
	sge::opencl::memory_object::image::planar &_divergence,
	sge::opencl::memory_object::image::planar &target)
{
	// We need those later on
	sge::opencl::memory_object::base *current_source = &p1_,*current_dest = &p2_;

	this->null_image(
		p1_);

	{
		profiler::scoped scoped_profiler(
			jacobi_profiler_,
			command_queue_);

		jacobi_kernel_.argument(
			sge::opencl::kernel::argument_index(
				0),
			_divergence);

		jacobi_kernel_.argument(
			sge::opencl::kernel::argument_index(
				2),
			boundary_);

		// Alpha
		jacobi_kernel_.argument(
			sge::opencl::kernel::argument_index(
				4),
			-(grid_scale_ * grid_scale_));

		// Beta (rbeta)
		jacobi_kernel_.argument(
			sge::opencl::kernel::argument_index(
				5),
			static_cast<cl_float>(1)/static_cast<cl_float>(4));

		for(unsigned i = 0; i < jacobi_iterations_; ++i)
		{
			jacobi_kernel_.argument(
				sge::opencl::kernel::argument_index(
					1),
				*current_source);

			jacobi_kernel_.argument(
				sge::opencl::kernel::argument_index(
					3),
				*current_dest);

			std::swap(
				current_source,
				current_dest);

			sge::opencl::command_queue::enqueue_kernel(
				command_queue_,
				jacobi_kernel_,
				fcppt::assign::make_array<std::size_t>
					(p1_.size()[0])
					(p1_.size()[1]).container(),
				fcppt::assign::make_array<std::size_t>
					(1)
					(1).container());
		}
	}

	profiler::scoped scoped_profiler(
		project_profiler_,
		command_queue_);

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		*current_source);

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		grid_scale_);

	// Temporary vector field (from which the divergence was calculated)
	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_velocity);

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		boundary_);

	gradient_and_subtract_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		target);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		gradient_and_subtract_kernel_,
		fcppt::assign::make_array<std::size_t>
			(p1_.size()[0])
			(p1_.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flakelib::simulation::stam::copy_image(
	sge::opencl::memory_object::image::planar &from,
	sge::opencl::memory_object::image::planar &to)
{
	FCPPT_ASSERT_PRE(
		from.size() == to.size());

	copy_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	copy_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		to);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		copy_image_kernel_,
		fcppt::assign::make_array<std::size_t>
			(from.size()[0])
			(from.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flakelib::simulation::stam::vector_magnitude(
	sge::opencl::memory_object::image::planar &from,
	sge::opencl::memory_object::image::planar &to,
	cl_float const scaling)
{
	FCPPT_ASSERT_PRE(
		from.size() == to.size());

	vector_magnitude_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	vector_magnitude_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		to);

	vector_magnitude_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		scaling);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		vector_magnitude_kernel_,
		fcppt::assign::make_array<std::size_t>
			(from.size()[0])
			(from.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flakelib::simulation::stam::laplace_residual(
	sge::opencl::memory_object::image::planar &lhs,
	sge::opencl::memory_object::image::planar &rhs,
	sge::opencl::memory_object::image::planar &to)
{
	FCPPT_ASSERT_PRE(
		lhs.size() == rhs.size());

	FCPPT_ASSERT_PRE(
		to.size() == rhs.size());

	laplace_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		lhs);

	laplace_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		rhs);

	laplace_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		to);

	laplace_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		boundary_);

	laplace_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		grid_scale_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		laplace_residual_kernel_,
		fcppt::assign::make_array<std::size_t>
			(lhs.size()[0])
			(lhs.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}
