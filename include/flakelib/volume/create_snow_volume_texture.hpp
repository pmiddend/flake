#ifndef FLAKELIB_VOLUME_CREATE_SNOW_VOLUME_TEXTURE_HPP_INCLUDED
#define FLAKELIB_VOLUME_CREATE_SNOW_VOLUME_TEXTURE_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/dim3.hpp>
#include <sge/renderer/texture/volume_unique_ptr.hpp>


namespace flakelib
{
namespace volume
{
FLAKELIB_SYMBOL
sge::renderer::texture::volume_unique_ptr
create_snow_volume_texture(
	sge::renderer::device &,
	sge::renderer::dim3 const &);
}
}

#endif

