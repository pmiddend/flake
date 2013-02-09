#include <flake/volume/snow_cover/object.hpp>
#include <flake/volume/snow_cover/scoped.hpp>
#include <sge/camera/base.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/matrix_conversion/world_projection.hpp>
#include <sge/cg/parameter/matrix/set.hpp>
#include <fcppt/make_cref.hpp>
#include <fcppt/assign/make_map.hpp>


flake::volume::snow_cover::scoped::scoped(
	flake::volume::snow_cover::object &_snow_cover,
	sge::renderer::context::core &_context)
:
	scoped_shader_(
		_context,
		_snow_cover.shader_),
	sampler_state_(
		_context,
		fcppt::assign::make_map<sge::renderer::state::core::sampler::const_object_ref_map>
			(
				_snow_cover.steep_texture_parameter_.stage(),
				fcppt::make_cref(
					   *_snow_cover.sampler_state_))
			(
				_snow_cover.flat_texture_parameter_.stage(),
				fcppt::make_cref(
					*_snow_cover.sampler_state_)))
{
	_snow_cover.mvp_parameter_.set(
		sge::camera::matrix_conversion::world_projection(
			_snow_cover.camera_.coordinate_system(),
			_snow_cover.camera_.projection_matrix()));
}

flake::volume::snow_cover::scoped::~scoped()
{
}
