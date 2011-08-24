#ifndef FLAKE_UPDATE_BOUNDARY_HPP_INCLUDED
#define FLAKE_UPDATE_BOUNDARY_HPP_INCLUDED

#include "boundary_type.hpp"
#include "scalar.hpp"
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/assert/pre.hpp>
#include <boost/mpl/assert.hpp>

namespace flake
{
template<typename Grid>
void
update_boundary(
	Grid &grid,
	flake::boundary_type::type const b)
{
	FCPPT_ASSERT_PRE(
		grid.size()[0] == grid.size()[1]);

	typedef typename
	Grid::dim
	grid_dim;

	typedef typename
	grid_dim::value_type
	grid_dim_value_type;

	BOOST_MPL_ASSERT_RELATION(
		grid_dim::dim_wrapper::value,==,2);

	grid_dim_value_type const N =
		grid.size()[0]-1;

	for(
		grid_dim_value_type i =
			static_cast<grid_dim_value_type>(1);
		i <= N;
		++i)
	{
		grid[grid_dim(0,i)] =
			b == boundary_type::velocity
			?
				-grid[grid_dim(1,i)]
			:
				grid[grid_dim(1,i)];

		grid[grid_dim(N,i)] =
			b == boundary_type::velocity
			?
				-grid[grid_dim(N,i)]
			:
				grid[grid_dim(N,i)];

		grid[grid_dim(i,0)] =
			b == boundary_type::velocity
			?
				-grid[grid_dim(i,1)]
			:
				grid[grid_dim(i,1)];

		grid[grid_dim(i,N)] =
			b == boundary_type::velocity
			?
				-grid[grid_dim(i,N)]
			:
				grid[grid_dim(i,N)];
	}

	grid[grid_dim(0,0)] =
		(grid[grid_dim(1,0)] + grid[grid_dim(0,1)])/static_cast<flake::scalar>(2.0f);

	grid[grid_dim(0,N)] =
		(grid[grid_dim(1,N)] + grid[grid_dim(0,N-1)])/static_cast<flake::scalar>(2.0f);
	grid[grid_dim(N,0)] =
		(grid[grid_dim(N-1,0)] + grid[grid_dim(N,1)])/static_cast<flake::scalar>(2.0f);
	grid[grid_dim(N,N)] =
		(grid[grid_dim(N-1,N)] + grid[grid_dim(N,N-1)])/static_cast<flake::scalar>(2.0f);
}
}

#endif
