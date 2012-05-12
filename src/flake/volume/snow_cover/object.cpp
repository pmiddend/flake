#include <sge/renderer/texture/planar.hpp>
#include <flake/shader/load_edited_string.hpp>
#include <sge/renderer/cg/loaded_texture.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/volume/snow_cover/object.hpp>
#include <sge/cg/parameter/scalar/set.hpp>
#include <sge/cg/parameter/vector/set.hpp>
#include <sge/cg/program/from_string_parameters.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/cg/loaded_program.hpp>


flake::volume::snow_cover::object::object(
	sge::camera::base &_camera,
	sge::renderer::device &_renderer,
	sge::cg::context::object &_cg_context,
	flake::shader::vertex_profile const &_cg_vertex_profile,
	flake::shader::pixel_profile const &_cg_pixel_profile,
	sge::renderer::vertex_declaration &_vertex_declaration,
	flake::volume::snow_cover::texture_repeats const &_texture_repeats,
	flake::volume::snow_cover::steep_texture const &_steep_texture,
	flake::volume::snow_cover::flat_texture const &_flat_texture,
	flake::volume::snow_cover::sun_direction const &_sun_direction)
:
	camera_(
		_camera),
	vertex_program_(
		sge::cg::program::from_string_parameters(
			_cg_context,
			sge::cg::program::source_type::text,
			_cg_vertex_profile.get(),
			flake::shader::load_edited_string(
				_renderer,
				_vertex_declaration,
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/snow_cover.cg"))),
			sge::cg::program::main_function(
				"vertex_main"),
			_renderer.cg_compile_options(
				_cg_context,
				_cg_vertex_profile.get()))),
	pixel_program_(
		sge::cg::program::from_string_parameters(
			_cg_context,
			sge::cg::program::source_type::text,
			_cg_pixel_profile.get(),
			flake::shader::load_edited_string(
				_renderer,
				_vertex_declaration,
				flake::media_path_from_string(
					FCPPT_TEXT("shaders/snow_cover.cg"))),
			sge::cg::program::main_function(
				"pixel_main"),
			_renderer.cg_compile_options(
				_cg_context,
				_cg_pixel_profile.get()))),
	loaded_vertex_program_(
		_renderer.load_cg_program(
			vertex_program_)),
	loaded_pixel_program_(
		_renderer.load_cg_program(
			pixel_program_)),
	steep_texture_(
		_steep_texture.get()),
	flat_texture_(
		_flat_texture.get()),
	loaded_steep_texture_(
		_renderer.load_cg_texture(
			pixel_program_.parameter(
				"steep_texture").object(),
			*steep_texture_)),
	loaded_flat_texture_(
		_renderer.load_cg_texture(
			pixel_program_.parameter(
				"flat_texture").object(),
			*flat_texture_)),
	mvp_parameter_(
		vertex_program_.parameter(
			"mvp")),
	texture_repeats_parameter_(
		pixel_program_.parameter(
			"texture_repeats")),
	sun_direction_parameter_(
		pixel_program_.parameter(
			"sun_direction"))
{
	sge::cg::parameter::vector::set(
		sun_direction_parameter_.object(),
		_sun_direction.get());

	sge::cg::parameter::scalar::set(
		texture_repeats_parameter_.object(),
		_texture_repeats.get());
}

flake::volume::snow_cover::object::~object()
{
}
