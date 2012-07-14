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
	scoped_shader_(
		_context,
		_snow_cover.shader_),
	steep_texture_filter_(
		_context,
		_snow_cover.steep_texture_parameter_.stage(),
		sge::renderer::texture::filter::trilinear()),
	flat_texture_filter_(
		_context,
		_snow_cover.flat_texture_parameter_.stage(),
		sge::renderer::texture::filter::trilinear())
{
	_snow_cover.mvp_parameter_.set(
		sge::camera::matrix_conversion::world_projection(
			_snow_cover.camera_.coordinate_system(),
			_snow_cover.camera_.projection_matrix()));

	sge::renderer::texture::set_address_mode2(
		_context,
		_snow_cover.steep_texture_parameter_.stage(),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::repeat));

	sge::renderer::texture::set_address_mode2(
		_context,
		_snow_cover.flat_texture_parameter_.stage(),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::repeat));
}

flake::volume::snow_cover::scoped::~scoped()
{
}
