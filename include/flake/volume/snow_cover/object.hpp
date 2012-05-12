#ifndef FLAKE_VOLUME_SNOW_COVER_OBJECT_HPP_INCLUDED
#define FLAKE_VOLUME_SNOW_COVER_OBJECT_HPP_INCLUDED

#include <flake/shader/pixel_profile.hpp>
#include <flake/shader/vertex_profile.hpp>
#include <flake/volume/snow_cover/flat_texture.hpp>
#include <flake/volume/snow_cover/scoped_fwd.hpp>
#include <flake/volume/snow_cover/steep_texture.hpp>
#include <flake/volume/snow_cover/sun_direction.hpp>
#include <flake/volume/snow_cover/texture_repeats.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/cg/context/object_fwd.hpp>
#include <sge/cg/parameter/named.hpp>
#include <sge/cg/program/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/cg/loaded_program_scoped_ptr.hpp>
#include <sge/renderer/cg/loaded_texture_scoped_ptr.hpp>
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
		sge::renderer::device &,
		sge::cg::context::object &,
		flake::shader::vertex_profile const &,
		flake::shader::pixel_profile const &,
		flake::volume::snow_cover::texture_repeats const &,
		flake::volume::snow_cover::steep_texture const &,
		flake::volume::snow_cover::flat_texture const &,
		flake::volume::snow_cover::sun_direction const &);

	~object();
private:
	friend class flake::volume::snow_cover::scoped;

	sge::camera::base &camera_;
	sge::cg::program::object vertex_program_;
	sge::cg::program::object pixel_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_vertex_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_pixel_program_;
	sge::renderer::texture::planar_shared_ptr steep_texture_;
	sge::renderer::texture::planar_shared_ptr flat_texture_;
	sge::renderer::cg::loaded_texture_scoped_ptr loaded_steep_texture_;
	sge::renderer::cg::loaded_texture_scoped_ptr loaded_flat_texture_;
	sge::cg::parameter::named mvp_parameter_;
	sge::cg::parameter::named texture_repeats_parameter_;
	sge::cg::parameter::named sun_direction_parameter_;
};
}
}
}

#endif

