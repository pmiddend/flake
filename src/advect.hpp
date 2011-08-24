#ifndef FLAKE_ADVECT_HPP_INCLUDED
#define FLAKE_ADVECT_HPP_INCLUDED

#include "boundary_type.hpp"
#include "update_boundary.hpp"
#include "ndim/on_border.hpp"
#include "ndim/clamp.hpp"
#include "scalar_duration.hpp"
#include "scalar.hpp"
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/container/grid/iterator_position.hpp>
#include <fcppt/container/grid/interpolate.hpp>
#include <fcppt/chrono/duration_impl.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/vector/basic.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/interpolation/linear_functor.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/assert.hpp>

namespace flake
{
template<typename Grid,typename VectorGrid>
void
advect(
	flake::boundary_type::type const b,
	Grid const &source,
	Grid &destination,
	VectorGrid const &velocity_field,
	flake::scalar_duration const &diff)
{
	flake::scalar const dt0 =
		diff.count();

	BOOST_MPL_ASSERT((
		boost::is_same<typename Grid::dim,typename VectorGrid::dim>));

	typedef typename
	Grid::dim
	grid_dim;

	typedef
	fcppt::math::vector::basic
	<
		typename grid_dim::value_type,
		typename grid_dim::dim_wrapper,
		typename grid_dim::storage_type
	>
	integer_vector_type;

	typedef typename
	VectorGrid::value_type
	float_vector_type;

	FCPPT_ASSERT_PRE(
		source.size() == destination.size());

	FCPPT_ASSERT_PRE(
		velocity_field.size() ==
		destination.size());

	typedef typename
	Grid::const_iterator
	grid_const_iterator;

	for(
		grid_const_iterator current_pixel =
			source.begin();
		current_pixel != source.end();
		++current_pixel)
	{
		grid_dim const current_position =
			fcppt::container::grid::iterator_position(
				source,
				current_pixel);

		if(ndim::on_border(source,current_position))
			continue;

		integer_vector_type const current_position_vector =
			fcppt::math::dim::structure_cast<integer_vector_type>(
				current_position);

		float_vector_type const backtracked_position =
			ndim::clamp(
				fcppt::math::vector::structure_cast<float_vector_type>(
					current_position_vector) -
				dt0 *
				velocity_field[
					fcppt::math::vector::structure_cast<grid_dim>(
						current_position_vector)],
				static_cast<flake::scalar>(
					0.5f),
				static_cast<flake::scalar>(
					source.size()[0]-1) +
				0.5f);

		destination[current_position] =
			fcppt::container::grid::interpolate(
				source,
				backtracked_position,
				fcppt::math::interpolation::linear_functor());
	}

	flake::update_boundary(
		destination,
		b);
}
}

#endif
