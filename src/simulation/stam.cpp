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

#include "stam.hpp"
#include "../media_path_from_string.hpp"
#include <sge/image2d/view/size.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image/algorithm/may_overlap.hpp>
#include <sge/opencl/program/source_string_sequence.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/scoped_planar_mapping.hpp>
#include <sge/opencl/memory_object/rect.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/io/stream_to_string.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/chrono/duration_impl.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/text.hpp>
#include <fcppt/optional.hpp>
#include <algorithm>

namespace
{
cl_image_format const
create_image_format(
	cl_channel_order const co,
	cl_channel_type const ct)
{
	cl_image_format result;
	result.image_channel_order = co;
	result.image_channel_data_type = ct;
	return result;
}
}

flake::simulation::stam::stam(
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_command_queue,
	flake::boundary_view const &boundary,
	sge::parse::json::object const &_config_file)
:
	command_queue_(
		_command_queue),
	v1_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_RG,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	v2_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_RG,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	temporary_v_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_RG,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	p1_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_R,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	p2_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_R,
			CL_FLOAT),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	boundary_(
		_context,
		CL_MEM_READ_WRITE,
		create_image_format(
			CL_INTENSITY,
			CL_UNORM_INT8),
		fcppt::math::dim::structure_cast<sge::opencl::memory_object::dim2>(
			sge::image2d::view::size(
				boundary.get())),
		sge::opencl::memory_object::image::planar_pitch(
			0)),
	main_program_(
		_context,
		fcppt::assign::make_container<sge::opencl::program::source_string_sequence>(
			fcppt::io::stream_to_string(
				*fcppt::make_unique_ptr<fcppt::io::cifstream>(
					flake::media_path_from_string(
						FCPPT_TEXT("kernels/stam.cl"))))),
		sge::opencl::program::build_parameters()),
	null_image_(
		main_program_,
		sge::opencl::kernel::name(
			"null_image")),
	copy_boundary_(
		main_program_,
		sge::opencl::kernel::name(
			"copy_boundary")),
	advect_(
		main_program_,
		sge::opencl::kernel::name(
			"advect")),
	apply_external_forces_(
		main_program_,
		sge::opencl::kernel::name(
			"apply_external_forces")),
	divergence_(
		main_program_,
		sge::opencl::kernel::name(
			"divergence")),
	jacobi_(
		main_program_,
		sge::opencl::kernel::name(
			"jacobi")),
	gradient_and_subtract_(
		main_program_,
		sge::opencl::kernel::name(
			"gradient_and_subtract")),
	copy_image_(
		main_program_,
		sge::opencl::kernel::name(
			"copy_image")),
	external_force_magnitude_(
		sge::parse::json::find_and_convert_member<cl_float>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("external-force-magnitude")))),
	grid_size_(
		sge::parse::json::find_and_convert_member<cl_float>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("grid-size")))),
	jacobi_iterations_(
		sge::parse::json::find_and_convert_member<unsigned>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("jacobi-iterations"))))
{
	sge::image2d::dim const boundary_dim =
		sge::image2d::view::size(
			boundary.get());

	fcppt::io::cout << FCPPT_TEXT("Resetting vector field v1\n");

	null_image_.argument(
		sge::opencl::kernel::argument_index(
			0),
		v1_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		null_image_,
		fcppt::assign::make_array<std::size_t>
			(boundary_dim[0])
			(boundary_dim[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());

	fcppt::io::cout << FCPPT_TEXT("Done\n");
	fcppt::io::cout << FCPPT_TEXT("Copying boundary to OpenCL buffer\n");

	{
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

	fcppt::io::cout << FCPPT_TEXT("Done\n");
}

sge::opencl::memory_object::image::planar &
flake::simulation::stam::vector_field()
{
	//return temporary_v_;
	return v1_;
}

void
flake::simulation::stam::update(
	flake::duration const &dt)
{
	// Copy boundary to v1
	this->copy_boundary(
		v1_);

	// Advect from v1 to v2
	this->advect(
		dt,
		v1_,
		v2_);

	// Apply forces inside v2
	this->apply_forces(
		v2_);

	// Calculate divergence of v2 to v1
	this->divergence(
		v2_,
		v1_);


	// Project v2, store result in v1
	this->project(
		v2_,
		v1_,
		v1_);

	/*
	// Calculate divergence of v2 to v1
	this->divergence(
		v1_,
		temporary_v_);
			*/
}

flake::simulation::stam::~stam()
{
}

void
flake::simulation::stam::copy_boundary(
	sge::opencl::memory_object::image::planar &target)
{
	copy_boundary_.argument(
		sge::opencl::kernel::argument_index(
			0),
		boundary_);

	copy_boundary_.argument(
		sge::opencl::kernel::argument_index(
			1),
		target);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		copy_boundary_,
		fcppt::assign::make_array<std::size_t>
			(target.size()[0])
			(target.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flake::simulation::stam::advect(
	flake::duration const &dt,
	sge::opencl::memory_object::image::planar &from,
	sge::opencl::memory_object::image::planar &to)
{
	advect_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	advect_.argument(
		sge::opencl::kernel::argument_index(
			1),
		to);

	advect_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_float>(
			dt.count()));

	advect_.argument(
		sge::opencl::kernel::argument_index(
			3),
		grid_size_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		advect_,
		fcppt::assign::make_array<std::size_t>
			(from.size()[0])
			(from.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flake::simulation::stam::apply_forces(
	sge::opencl::memory_object::image::planar &v)
{
	static bool force_applied = false;

	if(force_applied)
		return;

	// External forces
	// Apply external forces to v2 (inline)
	apply_external_forces_.argument(
		sge::opencl::kernel::argument_index(
			0),
		v);

	apply_external_forces_.argument(
		sge::opencl::kernel::argument_index(
			1),
		external_force_magnitude_);

	cl_uint const fan_width = 1; 

	apply_external_forces_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_uint>(
			v.size()[1]/2 - fan_width));

	apply_external_forces_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_uint>(
			v.size()[1]/2 + fan_width - 1));

	apply_external_forces_.argument(
		sge::opencl::kernel::argument_index(
			4),
		grid_size_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		apply_external_forces_,
		fcppt::assign::make_array<std::size_t>(/* 2* */fan_width).container(),
		fcppt::assign::make_array<std::size_t>(1).container());
	
	force_applied = true;
}

void
flake::simulation::stam::divergence(
	sge::opencl::memory_object::image::planar &from,
	sge::opencl::memory_object::image::planar &to)
{
	divergence_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	divergence_.argument(
		sge::opencl::kernel::argument_index(
			1),
		to);

	divergence_.argument(
		sge::opencl::kernel::argument_index(
			2),
		grid_size_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		divergence_,
		fcppt::assign::make_array<std::size_t>
			(from.size()[0])
			(from.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flake::simulation::stam::project(
	sge::opencl::memory_object::image::planar &v,
	sge::opencl::memory_object::image::planar &_divergence,
	sge::opencl::memory_object::image::planar &target)
{
	// Null p1 (initial solution guess)
	null_image_.argument(
		sge::opencl::kernel::argument_index(
			0),
		p1_);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		null_image_,
		fcppt::assign::make_array<std::size_t>
			(p1_.size()[0])
			(p1_.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());

	jacobi_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_divergence);

	sge::opencl::memory_object::base *current_source = &p1_,*current_dest = &p2_;

	// Alpha
	jacobi_.argument(
		sge::opencl::kernel::argument_index(
			3),
		- (grid_size_ * grid_size_));

	// Beta (rbeta)
	jacobi_.argument(
		sge::opencl::kernel::argument_index(
			4),
		static_cast<cl_float>(1)/static_cast<cl_float>(4));

	for(unsigned i = 0; i < jacobi_iterations_; ++i)
	{
		jacobi_.argument(
			sge::opencl::kernel::argument_index(
				1),
			*current_source);

		jacobi_.argument(
			sge::opencl::kernel::argument_index(
				2),
			*current_dest);

		std::swap(
			current_source,
			current_dest);

		sge::opencl::command_queue::enqueue_kernel(
			command_queue_,
			jacobi_,
			fcppt::assign::make_array<std::size_t>
				(p1_.size()[0])
				(p1_.size()[1]).container(),
			fcppt::assign::make_array<std::size_t>
				(1)
				(1).container());
	}

	gradient_and_subtract_.argument(
		sge::opencl::kernel::argument_index(
			0),
		*current_source);

	gradient_and_subtract_.argument(
		sge::opencl::kernel::argument_index(
			1),
		grid_size_);

	// Temporary vector field (from which the divergence was calculated)
	gradient_and_subtract_.argument(
		sge::opencl::kernel::argument_index(
			2),
		v);

	gradient_and_subtract_.argument(
		sge::opencl::kernel::argument_index(
			3),
		target);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		gradient_and_subtract_,
		fcppt::assign::make_array<std::size_t>
			(p1_.size()[0])
			(p1_.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flake::simulation::stam::copy_image(
	sge::opencl::memory_object::image::planar &from,
	sge::opencl::memory_object::image::planar &to)
{
	FCPPT_ASSERT_PRE(
		from.size() == to.size());

	copy_image_.argument(
		sge::opencl::kernel::argument_index(
			0),
		from);

	copy_image_.argument(
		sge::opencl::kernel::argument_index(
			1),
		to);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		copy_image_,
		fcppt::assign::make_array<std::size_t>
			(from.size()[0])
			(from.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}
