#include <flakelib/visualization/monitor/parent.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/visualization/arrow_vf/format.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <sge/opencl/program/source_string_sequence.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
// DEBUG
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/base_ref_sequence.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/memory_object/vertex_buffer.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/kernel/name.hpp>
#include <sge/opencl/kernel/argument_index.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/io/stream_to_string.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/text.hpp>

flakelib::visualization::monitor::parent::parent(
	sge::renderer::device &_renderer,
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_command_queue)
:
	renderer_(
		_renderer),
	context_(
		_context),
	command_queue_(
		_command_queue),
	vd_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<arrow_vf::format>())),
	conversion_program_(
		context_,
		fcppt::assign::make_container<sge::opencl::program::source_string_sequence>(
			fcppt::io::stream_to_string(
				*fcppt::make_unique_ptr<fcppt::io::cifstream>(
					flakelib::media_path_from_string(
						FCPPT_TEXT("kernels/vertex_buffer_conversions.cl"))))),
		sge::opencl::program::build_parameters()),
	image_to_vb_kernel_(
		conversion_program_,
		sge::opencl::kernel::name(
			"image_to_vb")),
	buffer_to_vb_kernel_(
		conversion_program_,
		sge::opencl::kernel::name(
			"buffer_to_vb")),
	arrow_shader_(
		sge::shader::object_parameters(
			renderer_,
			*vd_,
			sge::shader::vf_to_string<arrow_vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"initial_position",
					sge::shader::variable_type::uniform,
					sge::renderer::vector2(10.0f,10.0f)))
				((sge::shader::variable(
					"projection",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4(),
						sge::shader::matrix_flags::projection)))),
			sge::shader::sampler_sequence())
			.vertex_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/arrow/vertex.glsl")))
			.fragment_shader(
				flakelib::media_path_from_string(
					FCPPT_TEXT("shaders/arrow/fragment.glsl"))))
{
}

sge::renderer::vertex_declaration const &
flakelib::visualization::monitor::parent::vertex_declaration() const
{
	return *vd_;
}

sge::opencl::context::object &
flakelib::visualization::monitor::parent::context() const
{
	return context_;
}

void
flakelib::visualization::monitor::parent::to_vb(
	flakelib::buffer_or_image const &_buffer_or_image,
	sge::opencl::memory_object::vertex_buffer &_vb,
	monitor::grid_scale const &_grid_scale,
	monitor::arrow_scale const &_arrow_scale)
{
	if(_buffer_or_image.type() == typeid(sge::opencl::memory_object::image::planar *))
		this->image_to_vb(
			*_buffer_or_image.get<sge::opencl::memory_object::image::planar *>(),
			_vb,
			_grid_scale,
			_arrow_scale);
	else
		this->planar_buffer_to_vb(
			_buffer_or_image.get<flakelib::planar_buffer>(),
			_vb,
			_grid_scale,
			_arrow_scale);
}

sge::shader::object &
flakelib::visualization::monitor::parent::arrow_shader()
{
	return arrow_shader_;
}

sge::renderer::device &
flakelib::visualization::monitor::parent::renderer() const
{
	return renderer_;
}

flakelib::visualization::monitor::parent::~parent()
{
}

void
flakelib::visualization::monitor::parent::image_to_vb(
	sge::opencl::memory_object::image::planar &_image,
	sge::opencl::memory_object::vertex_buffer &_vb,
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

	// DEBUG
	fcppt::io::cerr() << FCPPT_TEXT("executing image_to_vb_kernel (image size ") << _image.size() << FCPPT_TEXT(")\n");

	// DEBUG
	command_queue_.finish();

	{
	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	sge::opencl::command_queue::dim2 global_dim;
	global_dim[0] = _image.size().w();
	global_dim[1] = _image.size().h();

	sge::opencl::command_queue::dim2 local_dim;
	local_dim[0] = 1;
	local_dim[1] = 1;

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		image_to_vb_kernel_,
		global_dim,
		local_dim);
	}

	fcppt::io::cerr() << FCPPT_TEXT("done\n");
}

void
flakelib::visualization::monitor::parent::planar_buffer_to_vb(
	flakelib::planar_buffer const &_buffer,
	sge::opencl::memory_object::vertex_buffer &_vb,
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
	global_dim[0] = static_cast<sge::opencl::command_queue::dim2::value_type>(_buffer.size().w());
	global_dim[1] = static_cast<sge::opencl::command_queue::dim2::value_type>(_buffer.size().h());

	sge::opencl::command_queue::dim2 local_dim;
	local_dim[0] = 1;
	local_dim[1] = 1;

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		buffer_to_vb_kernel_,
		global_dim,
		local_dim);
}
