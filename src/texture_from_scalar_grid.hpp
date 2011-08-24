#ifndef FLAKE_TEXTURE_FROM_SCALAR_GRID_HPP_INCLUDED
#define FLAKE_TEXTURE_FROM_SCALAR_GRID_HPP_INCLUDED

#include "scalar_grid2.hpp"
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/renderer/device.hpp>

namespace flake
{
sge::renderer::texture::planar_ptr const
texture_from_scalar_grid(
	sge::renderer::device &,
	flake::scalar_grid2 const &);
}

#endif
