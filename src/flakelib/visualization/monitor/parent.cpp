#include <flakelib/visualization/monitor/parent.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/visualization/monitor/arrow_vf/format.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <sge/opencl/program/source_string_sequence.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/memory_object/base_ref_sequence.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/kernel/name.hpp>
#include <sge/opencl/kernel/argument_index.hpp>
#include <sge/sprite/default_equal.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/io/stream_to_string.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/box/structure_cast.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/assert/pre.hpp>
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
	image_to_image_kernel_(
		conversion_program_,
		sge::opencl::kernel::name(
			"image_to_image")),
	buffer_to_image_kernel_(
		conversion_program_,
		sge::opencl::kernel::name(
			"buffer_to_image")),
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
					FCPPT_TEXT("shaders/arrow/fragment.glsl")))),
	sprite_system_(
		renderer_),
	children_(),
	window_manager_(
		children_,
		monitor::name(
			FCPPT_TEXT("velocity")),
		fcppt::math::box::structure_cast<monitor::rect>(
			renderer_.onscreen_target().viewport().get()))
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
	flakelib::planar_object const &_planar_object,
	sge::opencl::memory_object::buffer &_vb,
	monitor::grid_scale const &_grid_scale,
	monitor::arrow_scale const &_arrow_scale)
{
	if(_planar_object.type() == typeid(sge::opencl::memory_object::image::planar *))
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
flakelib::visualization::monitor::parent::to_texture(
	flakelib::planar_object const &_planar_object,
	sge::opencl::memory_object::image::planar &_texture,
	monitor::scaling_factor const &_scaling)
{
	if(_planar_object.type() == typeid(sge::opencl::memory_object::image::planar *))
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

flakelib::visualization::monitor::dummy_sprite::system &
flakelib::visualization::monitor::parent::sprite_system()
{
	return sprite_system_;
}

void
flakelib::visualization::monitor::parent::render()
{
	for(child_list::iterator it = children_.begin(); it != children_.end(); ++it)
		it->position(
			monitor::rect::vector::null());

	sprite_system_.render_all(
		sge::sprite::default_equal());

	for(child_list::iterator it = children_.begin(); it != children_.end(); ++it)
		it->render();
}

void
flakelib::visualization::monitor::parent::update()
{
	window_manager_.area(
		fcppt::math::box::structure_cast<monitor::rect>(
			renderer_.onscreen_target().viewport().get()));
	window_manager_.update();
}

flakelib::visualization::monitor::parent::~parent()
{
}

void
flakelib::visualization::monitor::parent::image_to_vb(
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
}

void
flakelib::visualization::monitor::parent::planar_buffer_to_vb(
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

void
flakelib::visualization::monitor::parent::image_to_image(
	sge::opencl::memory_object::image::planar &_input,
	sge::opencl::memory_object::image::planar &_output,
	monitor::scaling_factor const &_scaling)
{
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

	{
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
			image_to_image_kernel_,
			global_dim,
			local_dim);
	}
}

void
flakelib::visualization::monitor::parent::planar_buffer_to_image(
	flakelib::planar_buffer const &_input,
	sge::opencl::memory_object::image::planar &_output,
	monitor::scaling_factor const &_scaling)
{
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

	{
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
}

void
flakelib::visualization::monitor::parent::add_child(
	monitor::child &_child)
{
	children_.push_back(
		_child);
}

void
flakelib::visualization::monitor::parent::erase_child(
	monitor::child &_child)
{
	child_list::iterator it;
	for(
		it = children_.begin(); 
		it != children_.end() && &(*it) != &_child; 
		++it) ;

	FCPPT_ASSERT_PRE(
		it != children_.end());

	children_.erase(
		it);
}
