#ifndef FLAKE_DIFFUSE_HPP_INCLUDED
#define FLAKE_DIFFUSE_HPP_INCLUDED

#include "const_scalar_view.hpp"
#include "scalar_view.hpp"
#include "diffusion_coefficient.hpp"
#include "scalar_duration.hpp"

namespace flake
{
void
diffuse(
	flake::const_scalar_view const &source,
	flake::scalar_view const &destination,
	flake::diffusion_coefficient const &,
	flake::scalar_duration const &);
}

#endif
