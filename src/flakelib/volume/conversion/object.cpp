#include <flakelib/exception.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/volume/float_buffer_lock.hpp>
#include <flakelib/volume/conversion/object.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/move.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/math/dim/is_quadratic.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/fstream.hpp>
#include <fcppt/config/external_end.hpp>


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
		sge::opencl::command_queue::global_dim3(
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
		sge::opencl::command_queue::global_dim3(
			_input.size()));
}

flakelib::volume::unique_float_buffer_lock
flakelib::volume::conversion::object::raw_voxel_file_to_buffer(
	flakelib::buffer_pool::object &_buffer_pool,
	boost::filesystem::path const &_path,
	flakelib::volume::conversion::raw_voxel_file_dimension const &_raw_voxel_file_dimension)
{
	flakelib::volume::unique_float_buffer_lock result(
		fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
			fcppt::ref(
				_buffer_pool),
			sge::opencl::dim3(
				_raw_voxel_file_dimension.get(),
				_raw_voxel_file_dimension.get(),
				_raw_voxel_file_dimension.get())));

	boost::filesystem::fstream input_file(
		_path);

	if(!input_file.is_open())
		throw
			flakelib::exception(
				FCPPT_TEXT("Couldn't load \"")+
				fcppt::filesystem::path_to_string(
					_path)+
				FCPPT_TEXT("\""));

	sge::opencl::command_queue::scoped_buffer_mapping scoped_mapping(
		to_arrow_vb_kernel_->command_queue(),
		result->value().buffer(),
		sge::opencl::command_queue::map_flags::write,
		sge::opencl::memory_object::byte_offset(
			0u),
		result->value().buffer().byte_size(),
		sge::opencl::event::sequence());

	cl_float *fptr =
		static_cast<cl_float *>(
			scoped_mapping.ptr());

	for(
		std::size_t i = 0;
		i < result->value().size().content();
		++i)
	{
		char new_char;

		input_file.read(
			&new_char,
			1u);

		*fptr++ =
			new_char
			?
				1.0f
			:
				0.0f;
	}

	return
		fcppt::move(
			result);
}

flakelib::volume::conversion::object::~object()
{
}
