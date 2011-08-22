#ifndef FLAKE_TEXTURE_FROM_SCALAR_VIEW_HPP_INCLUDED
#define FLAKE_TEXTURE_FROM_SCALAR_VIEW_HPP_INCLUDED

#include "const_scalar_view.hpp"
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/device.hpp>

namespace flake
{
sge::renderer::texture::planar_ptr const
texture_from_scalar_view(
	sge::renderer::device &,
	flake::const_scalar_view const &);
}

#endif
