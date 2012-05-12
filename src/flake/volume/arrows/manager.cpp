#include <flake/media_path_from_string.hpp>
#include <flake/volume/arrows/manager.hpp>
#include <flake/volume/arrows/vf/format.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/cg/parameter/matrix/set.hpp>
#include <sge/cg/parameter/vector/set.hpp>
#include <sge/cg/program/from_file_parameters.hpp>
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
#include <fcppt/assign/make_container.hpp>
#include <fcppt/math/vector/arithmetic.hpp>


flake::volume::arrows::manager::manager(
	sge::renderer::device &_renderer,
	sge::cg::context::object &_cg_context,
	flake::shader::vertex_profile const &_cg_vertex_profile,
	flake::shader::pixel_profile const &_cg_pixel_profile,
	sge::camera::base &_camera)
:
	renderer_(
		_renderer),
	vertex_declaration_(
		_renderer.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<vf::format>())),
	camera_(
		_camera),
	vertex_program_(
		sge::cg::program::from_file_parameters(
			_cg_context,
			sge::cg::program::source_type::text,
			_cg_vertex_profile.get(),
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/volume_arrow.cg")),
			sge::cg::program::main_function(
				"vertex_main"),
			_renderer.cg_compile_options(
				_cg_context,
				_cg_vertex_profile.get()))),
	pixel_program_(
		sge::cg::program::from_file_parameters(
			_cg_context,
			sge::cg::program::source_type::text,
			_cg_pixel_profile.get(),
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/volume_arrow.cg")),
			sge::cg::program::main_function(
				"pixel_main"),
			_renderer.cg_compile_options(
				_cg_context,
				_cg_pixel_profile.get()))),
	loaded_vertex_program_(
		renderer_.load_cg_program(
			vertex_program_)),
	loaded_pixel_program_(
		renderer_.load_cg_program(
			pixel_program_)),
	mvp_parameter_(
		vertex_program_.parameter(
			"mvp")),
	camera_position_parameter_(
		vertex_program_.parameter(
			"camera_position")),
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

	sge::cg::parameter::vector::set(
		camera_position_parameter_.object(),
		-camera_.coordinate_system().position().get());

	sge::cg::parameter::matrix::set(
		mvp_parameter_.object(),
		sge::camera::matrix_conversion::world_projection(
			camera_.coordinate_system(),
			camera_.projection_matrix()));

	sge::renderer::cg::scoped_program
		scoped_vertex_program(
			_context,
			*loaded_vertex_program_),
		scoped_pixel_program(
			_context,
			*loaded_pixel_program_);

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
