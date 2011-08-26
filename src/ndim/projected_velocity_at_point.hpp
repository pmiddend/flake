#ifndef FLAKE_NDIM_PROJECTED_VELOCITY_AT_POINT_HPP_INCLUDED
#define FLAKE_NDIM_PROJECTED_VELOCITY_AT_POINT_HPP_INCLUDED

#include "null.hpp"
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <boost/mpl/identity.hpp>

namespace flake
{
namespace ndim
{
template<typename Result,typename Grid>
Result const
projected_velocity_at_point(
	Grid const &grid,
	typename Grid::dim position)
{
	typedef typename
	Grid::dim
	grid_dim;

	typedef typename
	Grid::value_type
	grid_value_type;

	typedef typename
	grid_dim::size_type
	grid_dim_size_type;

	Result result =
		ndim::null<Result>();

	for(
		grid_dim_size_type i = 0;
		i < grid_dim::dim_wrapper::value;
		++i)
	{
		position[i] += 1;
		grid_value_type const &right =
			grid[position];
		position[i] -= 2;
		grid_value_type const &left =
			grid[position];
		position[i] += 1;
		result[i] += right - left;
	}

	return result;
}
}
}

#endif
