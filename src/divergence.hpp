#ifndef FLAKE_DIVERGENCE_HPP_INCLUDED
#define FLAKE_DIVERGENCE_HPP_INCLUDED

#include "scalar.hpp"
#include "boundary_type.hpp"
#include "update_boundary.hpp"
#include "ndim/divergence_at_point.hpp"
#include "ndim/on_border.hpp"
#include <fcppt/assert/pre.hpp>
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/container/grid/iterator_position.hpp>
#include <fcppt/container/grid/is_square.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_floating_point.hpp>

namespace flake
{
template<typename VelocityGrid,typename ScalarGrid>
typename
boost::enable_if
<
	boost::is_floating_point<typename ScalarGrid::value_type>,
	void
>::type
divergence(
	VelocityGrid const &velocity,
	ScalarGrid &output)
{
	typedef typename
	VelocityGrid::const_iterator
	velocity_grid_const_iterator;

	typedef typename
	VelocityGrid::dim
	velocity_grid_dim;

	FCPPT_ASSERT_PRE(
		velocity.size() == output.size());

	FCPPT_ASSERT_PRE(
		fcppt::container::grid::is_square(
			velocity));

	flake::scalar const h =
		-0.5f * (1.0f / static_cast<flake::scalar>(velocity.size()[0]));

	for(
		velocity_grid_const_iterator it = velocity.begin();
		it != velocity.end();
		++it)
	{
		velocity_grid_dim const current_position =
			fcppt::container::grid::iterator_position(
				velocity,
				it);

		if(ndim::on_border(velocity,current_position))
			continue;

		output[current_position] =
			h *
			ndim::divergence_at_point(
				velocity,
				current_position);
	}

	flake::update_boundary(
		output,
		boundary_type::density);
}
}

#endif
