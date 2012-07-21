#include <flake/media_path_from_string.hpp>
#include <flake/volume/snow_cover/object.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/shader/load_edited_string.hpp>


flake::volume::snow_cover::object::object(
	sge::camera::base &_camera,
	sge::renderer::device &_renderer,
	sge::renderer::vertex_declaration &_vertex_declaration,
	sge::shader::context &_shader_context,
	flake::volume::snow_cover::texture_repeats const &_texture_repeats,
	flake::volume::snow_cover::steep_texture const &_steep_texture,
	flake::volume::snow_cover::flat_texture const &_flat_texture,
	flake::volume::snow_cover::sun_direction const &_sun_direction)
:
	camera_(
		_camera),
	shader_(
		_shader_context,
		_vertex_declaration,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/snow_cover.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/snow_cover.cg")))),
	steep_texture_(
		_steep_texture.get()),
	flat_texture_(
		_flat_texture.get()),
	steep_texture_parameter_(
		shader_,
		_renderer,
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"steep_texture")),
		sge::shader::parameter::planar_texture::optional_value(
			*steep_texture_)),
	flat_texture_parameter_(
		shader_,
		_renderer,
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"flat_texture")),
		sge::shader::parameter::planar_texture::optional_value(
			*flat_texture_)),
	mvp_parameter_(
		shader_.vertex_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"mvp")),
		sge::shader::parameter::is_projection_matrix(
			true),
		sge::renderer::matrix4()),
	texture_repeats_parameter_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"texture_repeats")),
		_texture_repeats.get()),
	sun_direction_parameter_(
		shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"sun_direction")),
		_sun_direction.get())
{
}

flake::volume::snow_cover::object::~object()
{
}
