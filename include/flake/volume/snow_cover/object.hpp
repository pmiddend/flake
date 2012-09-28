#ifndef FLAKE_VOLUME_SNOW_COVER_OBJECT_HPP_INCLUDED
#define FLAKE_VOLUME_SNOW_COVER_OBJECT_HPP_INCLUDED

#include <flake/volume/snow_cover/flat_texture.hpp>
#include <flake/volume/snow_cover/scoped_fwd.hpp>
#include <flake/volume/snow_cover/steep_texture.hpp>
#include <flake/volume/snow_cover/sun_direction.hpp>
#include <flake/volume/snow_cover/texture_repeats.hpp>
#include <sge/renderer/state/core/sampler/object_scoped_ptr.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/renderer/device/core_fwd.hpp>
#include <sge/renderer/vertex_declaration_fwd.hpp>
#include <sge/renderer/cg/loaded_program_scoped_ptr.hpp>
#include <sge/renderer/cg/loaded_texture_scoped_ptr.hpp>
#include <sge/shader/context.hpp>
#include <sge/shader/pair.hpp>
#include <sge/shader/parameter/matrix.hpp>
#include <sge/shader/parameter/planar_texture.hpp>
#include <sge/shader/parameter/scalar.hpp>
#include <sge/shader/parameter/vector.hpp>
#include <fcppt/noncopyable.hpp>


namespace flake
{
namespace volume
{
namespace snow_cover
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	object(
		sge::camera::base &_camera,
		sge::renderer::device::core &,
		sge::renderer::vertex_declaration &,
		sge::shader::context &,
		flake::volume::snow_cover::texture_repeats const &,
		flake::volume::snow_cover::steep_texture const &,
		flake::volume::snow_cover::flat_texture const &,
		flake::volume::snow_cover::sun_direction const &);

	~object();
private:
	friend class flake::volume::snow_cover::scoped;

	sge::camera::base &camera_;
	sge::shader::pair shader_;
	sge::renderer::texture::planar_shared_ptr steep_texture_;
	sge::renderer::texture::planar_shared_ptr flat_texture_;
	sge::shader::parameter::planar_texture steep_texture_parameter_;
	sge::shader::parameter::planar_texture flat_texture_parameter_;
	sge::shader::parameter::matrix<sge::renderer::scalar,4u,4u> mvp_parameter_;
	sge::shader::parameter::scalar<sge::renderer::scalar> texture_repeats_parameter_;
	sge::shader::parameter::vector<sge::renderer::scalar,3> sun_direction_parameter_;
	sge::renderer::state::core::sampler::object_scoped_ptr const sampler_state_;
};
}
}
}

#endif

