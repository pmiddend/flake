#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/monitor/planar_converter.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/variant/holds_type.hpp>


flakelib::monitor::planar_converter::planar_converter(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options)
:
	command_queue_(
		_command_queue),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/vertex_buffer_conversions.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	image_to_vb_kernel_(
		program_,
		sge::opencl::kernel::name(
			"image_to_vb")),
	buffer_to_vb_kernel_(
		program_,
		sge::opencl::kernel::name(
			"buffer_to_vb")),
	image_to_image_kernel_(
		program_,
		sge::opencl::kernel::name(
			"image_to_image")),
	buffer_to_image_kernel_(
		program_,
		sge::opencl::kernel::name(
			"buffer_to_image"))
{
}

void
flakelib::monitor::planar_converter::to_vb(
	flakelib::planar_object const &_planar_object,
	sge::opencl::memory_object::buffer &_vb,
	monitor::grid_scale const &_grid_scale,
	monitor::arrow_scale const &_arrow_scale)
{
	// TODO: Visitor!
	if(fcppt::variant::holds_type<sge::opencl::memory_object::image::planar *>(_planar_object))
		this->image_to_vb(
			*_planar_object.get<sge::opencl::memory_object::image::planar *>(),
			_vb,
			_grid_scale,
			_arrow_scale);
	else
		this->planar_buffer_to_vb(
			_planar_object.get<flakelib::planar_buffer>(),
			_vb,
			_grid_scale,
			_arrow_scale);
}

void
flakelib::monitor::planar_converter::to_texture(
	flakelib::planar_object const &_planar_object,
	sge::opencl::memory_object::image::planar &_texture,
	monitor::scaling_factor const &_scaling)
{
	// TODO: Visitor!
	if(fcppt::variant::holds_type<sge::opencl::memory_object::image::planar *>(_planar_object))
		this->image_to_image(
			*_planar_object.get<sge::opencl::memory_object::image::planar *>(),
			_texture,
			_scaling);
	else
		this->planar_buffer_to_image(
			_planar_object.get<flakelib::planar_buffer>(),
			_texture,
			_scaling);
}

flakelib::monitor::planar_converter::~planar_converter()
{
}

void
flakelib::monitor::planar_converter::image_to_vb(
	sge::opencl::memory_object::image::planar &_image,
	sge::opencl::memory_object::buffer &_vb,
	monitor::grid_scale const &_grid_scale,
	monitor::arrow_scale const &_arrow_scale)
{
	image_to_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_vb);

	image_to_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_image);

	image_to_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_grid_scale.get());

	image_to_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		_arrow_scale.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;

	mem_objects.push_back(
		&_vb);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	flakelib::cl::apply_kernel_to_planar_image(
		image_to_vb_kernel_,
		command_queue_,
		_image);
}

void
flakelib::monitor::planar_converter::planar_buffer_to_vb(
	flakelib::planar_buffer const &_buffer,
	sge::opencl::memory_object::buffer &_vb,
	monitor::grid_scale const &_grid_scale,
	monitor::arrow_scale const &_arrow_scale)
{
	buffer_to_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_vb);

	buffer_to_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_buffer.buffer());

	buffer_to_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		static_cast<cl_uint>(
			_buffer.size().w()));

	buffer_to_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		_grid_scale.get());

	buffer_to_vb_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		_arrow_scale.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_vb);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	sge::opencl::command_queue::dim2 global_dim;
	global_dim[0] =
		static_cast<sge::opencl::command_queue::dim2::value_type>(
			_buffer.size().w());
	global_dim[1] =
		static_cast<sge::opencl::command_queue::dim2::value_type>(
			_buffer.size().h());

	sge::opencl::command_queue::dim2 local_dim;
	local_dim[0] = 1;
	local_dim[1] = 1;

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		buffer_to_vb_kernel_,
		global_dim,
		local_dim);
}

void
flakelib::monitor::planar_converter::image_to_image(
	sge::opencl::memory_object::image::planar &_input,
	sge::opencl::memory_object::image::planar &_output,
	monitor::scaling_factor const &_scaling)
{
	FCPPT_ASSERT_PRE(
		_input.size() == _output.size());

	image_to_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_input);

	image_to_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_output);

	image_to_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_scaling.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_output);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	flakelib::cl::apply_kernel_to_planar_image(
		image_to_image_kernel_,
		command_queue_,
		_input);
}

void
flakelib::monitor::planar_converter::planar_buffer_to_image(
	flakelib::planar_buffer const &_input,
	sge::opencl::memory_object::image::planar &_output,
	monitor::scaling_factor const &_scaling)
{
	// TODO: Compare sizes?
	buffer_to_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_input.buffer());

	buffer_to_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_output);

	buffer_to_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_scaling.get());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&_output);

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	sge::opencl::command_queue::dim2 global_dim;
	global_dim[0] = _input.size().w();
	global_dim[1] = _input.size().h();

	sge::opencl::command_queue::dim2 local_dim;
	local_dim[0] = 1;
	local_dim[1] = 1;

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		buffer_to_image_kernel_,
		global_dim,
		local_dim);
}
