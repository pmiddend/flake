#ifndef FLAKE_UPDATE_BOUNDARY_HPP_INCLUDED
#define FLAKE_UPDATE_BOUNDARY_HPP_INCLUDED

#include "boundary_type.hpp"
#include "scalar_view.hpp"

namespace flake
{
void
update_boundary(
	flake::scalar_view const &,
	flake::boundary_type::type);
}

#endif
