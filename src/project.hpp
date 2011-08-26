#ifndef FLAKE_PROJECT_HPP_INCLUDED
#define FLAKE_PROJECT_HPP_INCLUDED

#include "divergence.hpp"
#include "projection.hpp"
#include "update_boundary.hpp"
#include "boundary_type.hpp"
#include "ndim/on_border.hpp"
#include "ndim/projected_velocity_at_point.hpp"
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/container/grid/iterator_position.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

namespace flake
{
template<typename ScalarGrid,typename VectorGrid>
void
project(
	ScalarGrid &divergence,
	ScalarGrid &projection,
	VectorGrid &velocity)
{
	typedef typename
	ScalarGrid::const_iterator
	scalar_grid_const_iterator;

	typedef typename
	ScalarGrid::dim
	scalar_grid_dim;

	typedef typename
	VectorGrid::value_type
	vector_grid_value_type;

	flake::divergence(
		velocity,
		divergence);

	flake::projection(
		divergence,
		projection);

	flake::scalar const h =
		-0.5f * (1.0f / static_cast<flake::scalar>(velocity.size()[0]));

	for(
		scalar_grid_const_iterator it = projection.begin();
		it != projection.end();
		++it)
	{
		scalar_grid_dim const current_position =
			fcppt::container::grid::iterator_position(
				projection,
				it);

		if(ndim::on_border(projection,current_position))
			continue;

		velocity[current_position] -=
			static_cast<flake::scalar>(0.5) / h *
			ndim::projected_velocity_at_point<vector_grid_value_type>(
				projection,
				current_position);
	}

	flake::update_boundary(
		velocity,
		boundary_type::velocity);
}
}

#endif
