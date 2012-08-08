#include <flake/media_path_from_string.hpp>
#include <flake/volume/arrows/manager.hpp>
#include <flake/volume/arrows/vf/format.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/cg/loaded_program.hpp>
#include <sge/renderer/cg/scoped_program.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/dest_blend_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/state/source_blend_func.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/shader/scoped_pair.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/vector/arithmetic.hpp>


flake::volume::arrows::manager::manager(
	sge::renderer::device &_renderer,
	sge::shader::context &_shader_context,
	sge::camera::base &_camera)
:
	renderer_(
		_renderer),
	vertex_declaration_(
		_renderer.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>())),
	camera_(
		_camera),
	shader_(
		_shader_context,
		*vertex_declaration_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/volume_arrow.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/volume_arrow.cg")))),
	mvp_parameter_(
		shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"mvp")),
		sge::shader::parameter::is_projection_matrix(
			true),
		sge::renderer::matrix4()),
	camera_position_parameter_(
		shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"camera_position")),
		sge::renderer::vector3()),
	children_()
{
}

void
flake::volume::arrows::manager::render(
	sge::renderer::context::object &_context)
{
	sge::renderer::state::scoped scoped_state(
		_context,
		sge::renderer::state::list
			(sge::renderer::state::bool_::enable_alpha_blending = true)
			(sge::renderer::state::source_blend_func::src_alpha)
			(sge::renderer::state::dest_blend_func::inv_src_alpha));

	sge::renderer::scoped_vertex_declaration scoped_vertex_declaration(
		_context,
		*vertex_declaration_);

	camera_position_parameter_.set(
		-camera_.coordinate_system().position().get());

	mvp_parameter_.set(
		sge::camera::matrix_conversion::world_projection(
			camera_.coordinate_system(),
			camera_.projection_matrix()));

	sge::shader::scoped_pair scoped_shader(
		_context,
		shader_);

	for(
		child_sequence::iterator it =
			children_.begin();
		it != children_.end();
		++it)
		it->render(
			_context);
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
