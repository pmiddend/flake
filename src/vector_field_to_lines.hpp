#ifndef FLAKE_VECTOR_FIELD_TO_LINES_HPP_INCLUDED
#define FLAKE_VECTOR_FIELD_TO_LINES_HPP_INCLUDED

#include <sge/line_drawer/object.hpp>
#include <sge/line_drawer/scoped_lock.hpp>
#include <sge/line_drawer/line_sequence.hpp>
#include <sge/line_drawer/line.hpp>
#include <sge/line_drawer/color.hpp>
#include <sge/image/color/any/object.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/renderer/scalar.hpp>
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <boost/mpl/identity.hpp>

namespace flake
{
namespace detail
{
template<typename Vector>
sge::renderer::vector3 const
to_vector3(
	Vector const &v)
{
	if(v.size() == 2)
		return
			sge::renderer::vector3(
				static_cast<sge::renderer::scalar>(
					v[0]),
				static_cast<sge::renderer::scalar>(
					v[1]),
				0.0f);
	return
		sge::renderer::vector3(
			static_cast<sge::renderer::scalar>(
				v[0]),
			static_cast<sge::renderer::scalar>(
				v[1]),
			static_cast<sge::renderer::scalar>(
				v[2]));
}

template<typename Grid>
void
vector_field_to_lines(
	sge::line_drawer::line_sequence &_lines,
	Grid const &_grid,
	typename Grid::value_type const &_translation,
	typename Grid::value_type const &_cell_size,
	typename boost::mpl::identity<typename Grid::value_type>::type::value_type const _arrow_length,
	sge::line_drawer::color const &_line_color,
	typename Grid::dim current_point_in_grid,
	typename Grid::dim::size_type const current_dimension)
{
	typedef typename
	Grid::value_type
	vector;

	typedef typename
	vector::value_type
	scalar;

	for(typename Grid::dim::value_type i = 0; i < _grid.size()[current_dimension]; ++i)
	{
		current_point_in_grid[current_dimension] = i;

		if(current_dimension == static_cast<typename Grid::dim::size_type>(current_point_in_grid.size()-1))
		{
			vector const
				center_of_cell =
					(_cell_size *
					fcppt::math::dim::structure_cast<vector>(
						current_point_in_grid)) +
					(_cell_size / static_cast<scalar>(2));

			_lines.push_back(
				sge::line_drawer::line(
					detail::to_vector3(
						_translation + center_of_cell),
					detail::to_vector3(
						_translation + center_of_cell + _arrow_length * _grid[current_point_in_grid]),
					sge::image::color::any::object(
						_line_color),
					sge::image::color::any::object(
						_line_color)));
		}
		else
		{
			detail::vector_field_to_lines(
				_lines,
				_grid,
				_translation,
				_cell_size,
				_arrow_length,
				_line_color,
				current_point_in_grid,
				static_cast<typename Grid::dim::size_type>(
					current_dimension+1));
		}
	}
}
}

template<typename Grid>
void
vector_field_to_lines(
	Grid const &_grid,
	sge::line_drawer::object &_line_drawer,
	typename Grid::value_type const &_translation,
	typename Grid::value_type const &_cell_size,
	typename boost::mpl::identity<typename Grid::value_type>::type::value_type const _arrow_length,
	sge::line_drawer::color const &_line_color)
{
	sge::line_drawer::scoped_lock scoped_lock_(
		_line_drawer);

	// Delete the old lines, ffs!
	scoped_lock_.value().clear();

	detail::vector_field_to_lines(
		scoped_lock_.value(),
		_grid,
		_translation,
		_cell_size,
		_arrow_length,
		_line_color,
		Grid::dim::null(),
		static_cast<typename Grid::dim::size_type>(
			0));
}
}

#endif
