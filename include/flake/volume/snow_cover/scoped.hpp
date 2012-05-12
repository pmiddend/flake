#ifndef FLAKE_VOLUME_SNOW_COVER_SCOPED_HPP_INCLUDED
#define FLAKE_VOLUME_SNOW_COVER_SCOPED_HPP_INCLUDED

#include <flake/volume/snow_cover/object_fwd.hpp>
#include <sge/renderer/cg/scoped_program.hpp>
#include <sge/renderer/cg/scoped_texture.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <fcppt/noncopyable.hpp>


namespace flake
{
namespace volume
{
namespace snow_cover
{
class scoped
{
FCPPT_NONCOPYABLE(
	scoped);
public:
	scoped(
		flake::volume::snow_cover::object &,
		sge::renderer::context::object &);

	~scoped();
private:
	sge::renderer::cg::scoped_texture steep_texture_;
	sge::renderer::cg::scoped_texture flat_texture_;
	sge::renderer::texture::filter::scoped steep_texture_filter_;
	sge::renderer::texture::filter::scoped flat_texture_filter_;
	sge::renderer::cg::scoped_program scoped_vertex_program_;
	sge::renderer::cg::scoped_program scoped_pixel_program_;
};
}
}
}

#endif

