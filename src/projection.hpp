#ifndef FLAKE_PROJECTION_HPP_INCLUDED
#define FLAKE_PROJECTION_HPP_INCLUDED

#include "scalar.hpp"
#include "ndim/null.hpp"
#include "ndim/sum_von_neumann.hpp"
#include "ndim/on_border.hpp"
#include "update_boundary.hpp"
#include "boundary_type.hpp"
#include <fcppt/assert/pre.hpp>
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/container/grid/iterator_position.hpp>
#include <fcppt/container/grid/is_square.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <algorithm>

namespace flake
{
template<typename Grid>
void
projection(
	Grid const &divergence,
	Grid &output)
{
	typedef typename
	Grid::dim
	grid_dim;

	typedef typename
	Grid::const_iterator
	grid_const_iterator;

	typedef typename
	Grid::value_type
	grid_value_type;

	FCPPT_ASSERT_PRE(
		divergence.size() == output.size());

	FCPPT_ASSERT_PRE(
		fcppt::container::grid::is_square(
			divergence));

	std::fill(
		output.begin(),
		output.end(),
		ndim::null<grid_value_type>());

	unsigned const iterations = 20;

	for(unsigned i = 0; i < iterations; ++i)
	{
		for(
			grid_const_iterator it = divergence.begin();
			it != divergence.end();
			++it)
		{
			grid_dim const current_position =
				fcppt::container::grid::iterator_position(
					divergence,
					it);

			if(ndim::on_border(divergence,current_position))
				continue;

			output[current_position] =
				(
					divergence[current_position] +
					ndim::sum_von_neumann(
						output,
						current_position)
				)
				/
				static_cast<flake::scalar>(
					1 << grid_dim::dim_wrapper::value);
		}

		flake::update_boundary(
			output,
			boundary_type::density);
	}

}
}

#endif
