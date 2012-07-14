#ifndef FLAKE_VOLUME_SNOW_COVER_SCOPED_HPP_INCLUDED
#define FLAKE_VOLUME_SNOW_COVER_SCOPED_HPP_INCLUDED

#include <flake/volume/snow_cover/object_fwd.hpp>
#include <flake/shader/scoped_pair.hpp>
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
	flake::shader::scoped_pair scoped_shader_;
	sge::renderer::texture::filter::scoped steep_texture_filter_;
	sge::renderer::texture::filter::scoped flat_texture_filter_;
};
}
}
}

#endif

