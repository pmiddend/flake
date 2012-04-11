#include <flake/media_path_from_string.hpp>
#include <flake/volume/arrows/manager.hpp>
#include <flake/volume/arrows/vf/format.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/object_parameters.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/vf_to_string.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/vector/arithmetic.hpp>


flake::volume::arrows::manager::manager(
	sge::renderer::device &_renderer,
	sge::camera::base &_camera)
:
	renderer_(
		_renderer),
	vertex_declaration_(
		_renderer.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>())),
	shader_(
		sge::shader::object_parameters(
			renderer_,
			*vertex_declaration_,
			sge::shader::vf_to_string<vf::format>(),
			fcppt::assign::make_container<sge::shader::variable_sequence>
				(sge::shader::variable(
					"mvp",
					sge::shader::variable_type::uniform,
					sge::shader::matrix(
						sge::renderer::matrix4::identity(),
						sge::shader::matrix_flags::projection)))
				(sge::shader::variable(
					"camera_position",
					sge::shader::variable_type::uniform,
					sge::renderer::vector3())),
			sge::shader::sampler_sequence())
			.vertex_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/volume_arrow/vertex.glsl")))
			.fragment_shader(
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/volume_arrow/fragment.glsl")))
			.name(
				FCPPT_TEXT("Arrows"))),
	camera_(
		_camera),
	children_()
{
}

void
flake::volume::arrows::manager::render()
{
	sge::renderer::state::scoped scoped_state(
		renderer_,
		sge::renderer::state::list
			(sge::renderer::state::bool_::enable_alpha_blending = true)
			(sge::renderer::state::source_blend_func::src_alpha)
			(sge::renderer::state::dest_blend_func::inv_src_alpha));

	sge::renderer::scoped_vertex_declaration scoped_vertex_declaration(
		renderer_,
		*vertex_declaration_);

	sge::shader::scoped scoped_shader(
		shader_,
		sge::shader::activate_everything());

	shader_.update_uniform(
		"camera_position",
		-camera_.coordinate_system().position().get());

	shader_.update_uniform(
		"mvp",
		sge::shader::matrix(
			sge::camera::matrix_conversion::world_projection(
				camera_.coordinate_system(),
				camera_.projection_matrix()),
			sge::shader::matrix_flags::projection));

	for(
		child_sequence::iterator it =
			children_.begin();
		it != children_.end();
		++it)
		it->render();
}

flake::volume::arrows::manager::~manager()
{
}

void
flake::volume::arrows::manager::add_child(
	arrows::object &_arrows)
{
	children_.push_back(
		_arrows);
}
