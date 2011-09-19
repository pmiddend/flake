#include "arrow.hpp"
#include "dummy_sprite/parameters.hpp"
#include "../simulation/base.hpp"
#include "../media_path_from_string.hpp"
#include "arrow_vf/format.hpp"
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/viewport_size.hpp>
#include <sge/renderer/projection/orthogonal_wh.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/part_index.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/sprite/render_one.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/dim2.hpp>
#include <sge/opencl/memory_object/base_ref_sequence.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/source_string_sequence.hpp>
#include <sge/image/colors.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <sge/shader/activate_everything.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/math/vector/output.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/io/stream_to_string.hpp>
#include <fcppt/text.hpp>

// DEBUG
/*
#include <cstdlib>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/iterator.hpp>
*/

flake::visualization::arrow::arrow(
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_command_queue,
	sge::renderer::device &_renderer,
	simulation::base &_simulation,
	sge::parse::json::object const &_config_file)
:
	command_queue_(
		_command_queue),
	renderer_(
		_renderer),
	simulation_(
		_simulation),
	vd_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<arrow_vf::format>())),
	vb_(
		renderer_.create_vertex_buffer(
			*vd_,
			sge::renderer::vf::dynamic::part_index(
				0u),
			static_cast<sge::renderer::size_type>(
				_simulation.vector_field().size().content() * 2),
			sge::renderer::resource_flags::readable)),
	cl_vb_(
		_context,
		*vb_,
		sge::opencl::memory_object::renderer_buffer_lock_mode::write_only),
	transfer_program_(
		_context,
		fcppt::assign::make_container<sge::opencl::program::source_string_sequence>(
			fcppt::io::stream_to_string(
				*fcppt::make_unique_ptr<fcppt::io::cifstream>(
					flake::media_path_from_string(
						FCPPT_TEXT("kernels/vector_field_to_arrows.cl"))))),
		sge::opencl::program::build_parameters()),
	transfer_kernel_(
		transfer_program_,
		sge::opencl::kernel::name(
			"fill_vb_with_arrows")),
	shader_(
		sge::shader::object_parameters(
			renderer_,
			*vd_,
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/arrow/vertex.glsl")),
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/arrow/fragment.glsl")),
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
			sge::shader::sampler_sequence())),
		sprite_system_(
			renderer_),
		sprite_object_(
			dummy_sprite::parameters()
				.pos(
					sge::renderer::vector2(10.0f,10.0f))
				.size(
					sge::parse::json::find_and_convert_member<dummy_sprite::object::unit>(
						_config_file,
						sge::parse::json::string_to_path(
							FCPPT_TEXT("grid-size"))) *
					fcppt::math::dim::structure_cast<dummy_sprite::object::dim>(
						simulation_.vector_field().size()))
				.any_color(
					sge::image::colors::black())
				.elements())
{
	transfer_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		cl_vb_);

	transfer_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		sge::parse::json::find_and_convert_member<cl_float>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("grid-size"))));

	transfer_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		sge::parse::json::find_and_convert_member<cl_float>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("visualization/arrow-length"))));
}

void
flake::visualization::arrow::update(
	flake::duration const &)
{
	transfer_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		simulation_.vector_field());

	sge::opencl::memory_object::base_ref_sequence mem_objects;
	mem_objects.push_back(
		&cl_vb_);

	{
	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		mem_objects);

	sge::opencl::command_queue::dim2 global_dim;
	global_dim[0] = simulation_.vector_field().size().w();
	global_dim[1] = simulation_.vector_field().size().h();

	sge::opencl::command_queue::dim2 local_dim;
	local_dim[0] = 1;
	local_dim[1] = 1;

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		transfer_kernel_,
		global_dim,
		local_dim);
	}

	/* DEBUG
	{
		sge::renderer::scoped_vertex_lock scoped_vb(
			*vb_,
			sge::renderer::lock_mode::readwrite);

		typedef
		sge::renderer::vf::view<arrow_vf::part>
		vertex_view;

		vertex_view const vertices(
			scoped_vb.value());

		for(
			vertex_view::iterator vb_it(
				vertices.begin());
			vb_it != vertices.end();
			vb_it++)
		{
			fcppt::io::cerr << vb_it->get<arrow_vf::position>() << FCPPT_TEXT("\n");
		}
	}

	std::exit(0);
	*/
}

void
flake::visualization::arrow::render()
{
	sge::sprite::render_one(
		sprite_system_,
		sprite_object_);

	// Activate the shader and the vertex declaration
	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_everything());

	sge::renderer::scoped_vertex_buffer scoped_vb(
		renderer_,
		*vb_);

	shader_.update_uniform(
		"projection",
		sge::shader::matrix(
			sge::renderer::projection::orthogonal_wh(
				fcppt::math::dim::structure_cast<sge::renderer::projection::dim>(
						sge::renderer::viewport_size(
							renderer_)),
				sge::renderer::projection::near(0.0f),
				sge::renderer::projection::far(10.0f)),
			sge::shader::matrix_flags::projection));

	renderer_.render_nonindexed(
		sge::renderer::first_vertex(
			0),
		sge::renderer::vertex_count(
			vb_->size()),
		sge::renderer::nonindexed_primitive_type::line);
}

sge::renderer::state::list const
flake::visualization::arrow::render_states() const
{
	return
		sge::renderer::state::list
			(sge::renderer::state::bool_::clear_back_buffer = true)
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::white());
}

flake::visualization::arrow::~arrow()
{
}
