#ifndef FLAKE_SCALAR_GRID2_HPP_INCLUDED
#define FLAKE_SCALAR_GRID2_HPP_INCLUDED

#include "scalar.hpp"
#include <fcppt/container/grid/object_fwd.hpp>

namespace flake
{
typedef
fcppt::container::grid::object
<
	flake::scalar,
	2
>
scalar_grid2;
}

#endif
