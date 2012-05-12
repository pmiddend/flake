#ifndef FLAKE_VOLUME_SNOW_COVER_FLAT_TEXTURE_HPP_INCLUDED
#define FLAKE_VOLUME_SNOW_COVER_FLAT_TEXTURE_HPP_INCLUDED

#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flake
{
namespace volume
{
namespace snow_cover
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::texture::planar_shared_ptr,
	flat_texture);
}
}
}

#endif

