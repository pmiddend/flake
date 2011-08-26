#ifndef FLAKE_NDIM_DIVERGENCE_AT_POINT_HPP_INCLUDED
#define FLAKE_NDIM_DIVERGENCE_AT_POINT_HPP_INCLUDED

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
template<typename Grid>
typename
boost::mpl::identity<typename Grid::value_type>::type::value_type
divergence_at_point(
	Grid const &grid,
	typename Grid::dim position)
{
	typedef typename
	Grid::value_type
	grid_value_type;

	typedef typename
	grid_value_type::value_type
	grid_value_type_value_type;

	typedef typename
	Grid::dim
	grid_dim;

	typedef typename
	grid_dim::size_type
	grid_dim_size_type;

	grid_value_type_value_type result =
		ndim::null<grid_value_type_value_type>();

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
		result += right[i] - left[i];
	}

	return result;
}
}
}

#endif
