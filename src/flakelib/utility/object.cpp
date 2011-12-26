#include <flakelib/media_path_from_string.hpp>
#include <flakelib/utility/object.hpp>
#include <flakelib/buffer/planar_view.hpp>
#include <flakelib/buffer/linear_view.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/size_type.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cmath>
#include <fcppt/config/external_end.hpp>


flakelib::utility::object::object(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options)
:
	command_queue_(
		_command_queue),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/utility.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	copy_float_buffer_kernel_(
		program_,
		sge::opencl::kernel::name(
			"copy_float_buffer")),
	null_float_buffer_kernel_(
		program_,
		sge::opencl::kernel::name(
			"null_float_buffer")),
	generate_float_oscillation_kernel_(
		program_,
		sge::opencl::kernel::name("generate_float_oscillation")),
	frobenius_norm_tile_kernel_(
		program_,
		sge::opencl::kernel::name("frobenius_norm_tile")),
	planar_vector_magnitude_kernel_(
		program_,
		sge::opencl::kernel::name("planar_vector_magnitude"))
{
}

void
flakelib::utility::object::null_buffer(
	buffer::linear_view<cl_float> const &_f)
{
	null_float_buffer_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_f.buffer());

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		null_float_buffer_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_f.size()).container());
}

void
flakelib::utility::object::copy_buffer(
	utility::copy_from const &_from,
	utility::copy_to const &_to,
	utility::multiplier const &_multiplier)
{
	copy_float_buffer_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_from.get().buffer());

	copy_float_buffer_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_to.get().buffer());

	copy_float_buffer_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_multiplier.get());

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		copy_float_buffer_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_from.get().size()).container());
}

#if 0
void
flakelib::utility::object::planar_vector_magnitude(
	utility::from const &_from,
	utility::to const &_to,
	utility::multiplier const &_multiplier)
{
	planar_vector_magnitude_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_from.get());

	planar_vector_magnitude_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_to.get());

	planar_vector_magnitude_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_multiplier.get());

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		planar_vector_magnitude_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_from.get().buffer().size()[0])
			(_from.get().buffer().size()[1]).container(),
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(8)
			(8).container());
}
#endif

void
flakelib::utility::object::generate_oscillation(
	buffer::planar_view<cl_float> const &_f)
{
	generate_float_oscillation_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_f.buffer());

	generate_float_oscillation_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		static_cast<cl_int>(
			_f.size().w()));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		generate_float_oscillation_kernel_,
		fcppt::assign::make_array<sge::opencl::memory_object::size_type>
			(_f.size()[0])
			(_f.size()[1]).container());
}

cl_float
flakelib::utility::object::frobenius_norm(
	buffer::linear_view<cl_float> const &_scalar_float_buffer)
{
	std::size_t const work_group_size =
		frobenius_norm_tile_kernel_.work_group_size(
			command_queue_.device());

	sge::opencl::memory_object::buffer partial_results(
		command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::write),
		sge::opencl::memory_object::byte_size(
			work_group_size * sizeof(cl_float)));

	frobenius_norm_tile_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_scalar_float_buffer.buffer());

	frobenius_norm_tile_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		partial_results);

	frobenius_norm_tile_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_int>(
			_scalar_float_buffer.size()));

	frobenius_norm_tile_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		static_cast<cl_int>(
			_scalar_float_buffer.size() / work_group_size + 1u));

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		frobenius_norm_tile_kernel_,
		fcppt::assign::make_array<std::size_t>
			(work_group_size).container(),
		fcppt::assign::make_array<std::size_t>
			(work_group_size).container());

	sge::opencl::command_queue::scoped_buffer_mapping scoped_lock(
		command_queue_,
		partial_results,
		CL_MAP_READ,
		sge::opencl::memory_object::byte_offset(
			0),
		sge::opencl::memory_object::byte_size(
			partial_results.byte_size()));

	cl_float result =
		0.0f;

	for(std::size_t i = 0; i < work_group_size; ++i)
		result +=
			static_cast<cl_float const *>(
				scoped_lock.ptr())[i];

	return
		std::sqrt(
			result);
}

flakelib::utility::object::~object()
{
}
