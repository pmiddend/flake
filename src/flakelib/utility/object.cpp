#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
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
	sge::opencl::command_queue::object &_command_queue)
:
	command_queue_(
		_command_queue),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/utility.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters())),
	copy_image_kernel_(
		program_,
		sge::opencl::kernel::name(
			"copy_image")),
	null_image_kernel_(
		program_,
		sge::opencl::kernel::name(
			"null_image")),
	generate_oscillation_kernel_(
		program_,
		sge::opencl::kernel::name("generate_oscillation")),
	frobenius_norm_tile_kernel_(
		program_,
		sge::opencl::kernel::name("frobenius_norm_tile")),
	frobenius_norm_kernel_(
		program_,
		sge::opencl::kernel::name("frobenius_norm")),
	planar_vector_magnitude_kernel_(
		program_,
		sge::opencl::kernel::name("planar_vector_magnitude"))
{
}

void
flakelib::utility::object::null_image(
	sge::opencl::memory_object::image::planar &_f)
{
	null_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_f);

	flakelib::cl::apply_kernel_to_planar_image(
		null_image_kernel_,
		command_queue_,
		_f);
}

void
flakelib::utility::object::copy_image(
	utility::from const &_from,
	utility::to const &_to,
	utility::multiplier const &_multiplier)
{
	copy_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_from.get());

	copy_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_to.get());

	copy_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_multiplier.get());

	flakelib::cl::apply_kernel_to_planar_image(
		copy_image_kernel_,
		command_queue_,
		_from.get());
}

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

	flakelib::cl::apply_kernel_to_planar_image(
		planar_vector_magnitude_kernel_,
		command_queue_,
		_from.get());
}

void
flakelib::utility::object::generate_oscillation(
	sge::opencl::memory_object::image::planar &_f)
{
	generate_oscillation_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_f);

	flakelib::cl::apply_kernel_to_planar_image(
		generate_oscillation_kernel_,
		command_queue_,
		_f);
}

cl_float
flakelib::utility::object::frobenius_norm(
	sge::opencl::memory_object::image::planar &_image)
{
	std::size_t const work_group_size =
		frobenius_norm_kernel_.work_group_size(
			command_queue_.device());

	//fcppt::io::cerr() << FCPPT_TEXT("Work group size: ") << work_group_size << FCPPT_TEXT("\n");

	// NOTE: This is optimized for square work group dimensions and thus
	// for square images.

	// We can't round up here because the work group size could be
	// surpassed
	std::size_t const rounded_down_root =
		static_cast<std::size_t>(
			std::sqrt(
				static_cast<double>(
					work_group_size)));

	/*
	fcppt::io::cerr()
		<< FCPPT_TEXT("Group size is: ")
		<< rounded_down_root << FCPPT_TEXT("x") << rounded_down_root
		<< FCPPT_TEXT("\n");
		*/

	sge::opencl::memory_object::buffer partial_results(
		command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::write),
		sge::opencl::memory_object::byte_size(
			work_group_size * sizeof(cl_float)));

	frobenius_norm_tile_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_image);

	frobenius_norm_tile_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		partial_results);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		frobenius_norm_tile_kernel_,
		fcppt::assign::make_array<std::size_t>
			(rounded_down_root)
			(rounded_down_root).container(),
		fcppt::assign::make_array<std::size_t>
			(rounded_down_root)
			(rounded_down_root).container());

	sge::opencl::memory_object::buffer total_result(
		command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::write) | sge::opencl::memory_object::flags::read,
		sge::opencl::memory_object::byte_size(
			sizeof(cl_float)));

	frobenius_norm_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		partial_results);

	frobenius_norm_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		static_cast<cl_uint>(
			rounded_down_root * rounded_down_root));

	frobenius_norm_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		total_result);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		frobenius_norm_kernel_,
		fcppt::assign::make_array<std::size_t>
			(1).container(),
		fcppt::assign::make_array<std::size_t>
			(1).container());

	sge::opencl::command_queue::scoped_buffer_mapping scoped_lock(
		command_queue_,
		total_result,
		CL_MAP_READ,
		sge::opencl::memory_object::byte_offset(
			0),
		sge::opencl::memory_object::byte_size(
			sizeof(cl_float)));

	return
		std::sqrt(
			*static_cast<cl_float *>(
				scoped_lock.ptr()));
}

flakelib::utility::object::~object()
{
}
