#ifndef FLAKE_NDIM_SUM_VON_NEUMANN_HPP_INCLUDED
#define FLAKE_NDIM_SUM_VON_NEUMANN_HPP_INCLUDED

#include "null.hpp"
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/math/vector/arithmetic.hpp>

namespace flake
{
namespace ndim
{
template<typename Grid>
typename Grid::value_type
sum_von_neumann(
	Grid const &grid,
	typename Grid::dim position)
{
	typedef typename
	Grid::value_type
	grid_value_type;

	typedef typename
	Grid::dim
	grid_dim;

	typedef typename
	grid_dim::size_type
	grid_dim_size_type;

	grid_value_type result =
		ndim::null<grid_value_type>();

	for(
		grid_dim_size_type i = 0;
		i < grid_dim::dim_wrapper::value;
		++i)
	{
		position[i] += 1;
		result += grid[position];
		position[i] -= 2;
		result += grid[position];
		position[i] += 1;
	}

	return result;
}
}
}

#endif
