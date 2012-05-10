#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/is_quadratic.hpp>
#include <fcppt/math/vector/object_impl.hpp>


flakelib::volume::conversion::object::object(
	cl::program_context const &_program_context)
:
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/volume/conversion.cl")),
		_program_context.compiler_flags()),
	to_arrow_vb_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"to_arrow_vb"))),
	float_view_to_flat_volume_texture_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"float_view_to_flat_volume_texture")))
{
}

void
flakelib::volume::conversion::object::to_arrow_vb(
	volume::float4_view const &_input,
	sge::opencl::memory_object::buffer &_output,
	conversion::arrow_scale const &_arrow_scale,
	conversion::origin const &_origin,
	conversion::grid_scale const &_grid_scale)
{
        sge::opencl::memory_object::base_ref_sequence mem_objects;

	mem_objects.push_back(
		&_output);

        sge::opencl::memory_object::scoped_objects scoped_vb(
		to_arrow_vb_kernel_->command_queue(),
		mem_objects);

	to_arrow_vb_kernel_->buffer_argument(
		"input",
		_input.buffer());

	to_arrow_vb_kernel_->buffer_argument(
		"output",
		_output);

	to_arrow_vb_kernel_->numerical_argument(
		"input_line_pitch",
		static_cast<cl_uint>(
			_input.size().w()));

	to_arrow_vb_kernel_->numerical_argument(
		"arrow_scale",
		_arrow_scale.get());

	to_arrow_vb_kernel_->numerical_argument(
		"grid_scale",
		_grid_scale.get());

	to_arrow_vb_kernel_->numerical_argument(
		"origin_x",
		_origin.get().x());

	to_arrow_vb_kernel_->numerical_argument(
		"origin_y",
		_origin.get().y());

	to_arrow_vb_kernel_->numerical_argument(
		"origin_z",
		_origin.get().z());

	to_arrow_vb_kernel_->enqueue_automatic(
		flakelib::cl::global_dim3(
			_input.size()));
}

void
flakelib::volume::conversion::object::float_view_to_flat_volume_texture(
	volume::float_view const &_input,
	sge::opencl::memory_object::image::planar &_output,
	conversion::scaling_factor const &_scaling_factor,
	conversion::constant_addition const &_constant_addition)
{
	FCPPT_ASSERT_PRE(
		fcppt::math::dim::is_quadratic(
			_input.size()));

	sge::opencl::memory_object::base_ref_sequence mem_objects;

	mem_objects.push_back(
		&_output);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		float_view_to_flat_volume_texture_kernel_->command_queue(),
		mem_objects);

	float_view_to_flat_volume_texture_kernel_->buffer_argument(
		"input",
		_input.buffer());

	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"input_line_pitch",
		static_cast<cl_uint>(
			_input.size().w()));

	float_view_to_flat_volume_texture_kernel_->planar_image_argument(
		"output",
		_output);

	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"scaling_factor",
		_scaling_factor.get());

	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"constant_addition",
		_constant_addition.get());

	// This is the place where the assertion above comes in.
	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"slice_edge_size",
		static_cast<cl_uint>(
			_input.size().w()));

	float_view_to_flat_volume_texture_kernel_->numerical_argument(
		"slices_per_row",
		static_cast<cl_uint>(
			_output.size().w() / _input.size().w()));

	float_view_to_flat_volume_texture_kernel_->enqueue_automatic(
		flakelib::cl::global_dim3(
			_input.size()));
}

flakelib::volume::conversion::object::~object()
{
}
