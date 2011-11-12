#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/text.hpp>


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
		sge::opencl::kernel::name("generate_oscillation"))
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
	utility::to const &_to)
{
	copy_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_from.get());

	copy_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_to.get());

	flakelib::cl::apply_kernel_to_planar_image(
		copy_image_kernel_,
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

/*
cl_float
flakelib::utility::object::frobenius_norm(
	sge::opencl::memory_object::image::planar &_image)
{
	sge::opencl::memory_object::buffer partial_results(
		command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::write),
		sge::opencl::memory_object::byte_size(
			));
}
*/

flakelib::utility::object::~object()
{
}
