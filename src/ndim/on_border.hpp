#ifndef FLAKE_NDIM_ON_BORDER_HPP_INCLUDED
#define FLAKE_NDIM_ON_BORDER_HPP_INCLUDED

#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

namespace flake
{
namespace ndim
{
template<typename Grid>
bool
on_border(
	Grid const &grid,
	typename Grid::dim const &position)
{
	typedef typename
	Grid::dim::size_type
	grid_size_type;

	for(grid_size_type i = 0; i < Grid::dim::dim_wrapper::value; ++i)
		if(position[i] == 0 || position[i] == (grid.size()[i] - 1))
			return true;
	return false;
}
}
}

#endif
