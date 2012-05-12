#include <flake/volume/snow_cover/object.hpp>
#include <flake/volume/snow_cover/scoped.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/cg/parameter/matrix/set.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/set_address_mode2.hpp>
#include <sge/renderer/texture/filter/trilinear.hpp>


flake::volume::snow_cover::scoped::scoped(
	flake::volume::snow_cover::object &_snow_cover,
	sge::renderer::context::object &_context)
:
	steep_texture_(
		_context,
		*_snow_cover.loaded_steep_texture_),
	flat_texture_(
		_context,
		*_snow_cover.loaded_flat_texture_),
	steep_texture_filter_(
		_context,
		steep_texture_.stage(),
		sge::renderer::texture::filter::trilinear()),
	flat_texture_filter_(
		_context,
		flat_texture_.stage(),
		sge::renderer::texture::filter::trilinear()),
	scoped_vertex_program_(
		_context,
		*_snow_cover.loaded_vertex_program_),
	scoped_pixel_program_(
		_context,
		*_snow_cover.loaded_pixel_program_)
{
	sge::cg::parameter::matrix::set(
		_snow_cover.mvp_parameter_.object(),
		sge::camera::matrix_conversion::world_projection(
			_snow_cover.camera_.coordinate_system(),
			_snow_cover.camera_.projection_matrix()));

	sge::renderer::texture::set_address_mode2(
		_context,
		steep_texture_.stage(),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::repeat));

	sge::renderer::texture::set_address_mode2(
		_context,
		flat_texture_.stage(),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::repeat));
}

flake::volume::snow_cover::scoped::~scoped()
{
}
