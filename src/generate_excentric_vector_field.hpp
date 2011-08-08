#ifndef FLAKE_GENERATE_EXCENTRIC_VECTOR_FIELD_HPP_INCLUDED
#define FLAKE_GENERATE_EXCENTRIC_VECTOR_FIELD_HPP_INCLUDED

#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/vector/normalize.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/container/grid/object_impl.hpp>

#include <iostream>
#include <fcppt/math/dim/output.hpp>

namespace flake
{
namespace detail
{
template<typename Grid>
void
generate_excentric_vector_field(
	Grid &grid,
	typename Grid::value_type const &reference_point,
	typename Grid::value_type const &_cell_size,
	typename Grid::dim current_point_in_grid,
	typename Grid::dim::size_type const current_dimension)
{
	typedef typename 
	boost::mpl::identity<typename Grid::value_type>::type::value_type
	scalar;

	for(typename Grid::dim::value_type i = 0; i < grid.size()[current_dimension]; ++i)
	{
		current_point_in_grid[current_dimension] = i;

		if(current_dimension == static_cast<typename Grid::dim::size_type>(current_point_in_grid.size()-1))
			grid[current_point_in_grid] = 
				fcppt::math::vector::normalize(
					_cell_size * 
					fcppt::math::dim::structure_cast<typename Grid::value_type>(
						current_point_in_grid) + (_cell_size / static_cast<scalar>(2)) - 
					reference_point);
		else
			detail::generate_excentric_vector_field(
				grid,
				reference_point,
				_cell_size,
				current_point_in_grid,
				static_cast<typename Grid::dim::size_type>(
					current_dimension+1));
	}
}
}

template<typename Grid>
Grid const
generate_excentric_vector_field(
	typename Grid::dim const &d,
	typename Grid::value_type const &reference_point,
	typename Grid::value_type const &cell_size)
{
	Grid result(
		d);

	detail::generate_excentric_vector_field(
		result,
		reference_point,
		cell_size,
		Grid::dim::null(),
		static_cast<typename Grid::dim::size_type>(
			0));

	return result;
}
}

#endif
