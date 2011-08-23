#ifndef FLAKE_ADVECT_HPP_INCLUDED
#define FLAKE_ADVECT_HPP_INCLUDED

#include "boundary_type.hpp"
#include "update_boundary.hpp"
#include "const_scalar_view.hpp"
#include "scalar_view.hpp"
#include "vector2_grid.hpp"
#include "scalar_duration.hpp"
#include "mizuiro_dimension_to_fcppt_dim.hpp"
#include "mizuiro_dimension_to_fcppt_vector_type.hpp"
#include "mizuiro_dimension_to_fcppt_vector.hpp"
#include <mizuiro/image/iterator_position.hpp>
#include <mizuiro/color/channel/luminance.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/vector/arithmetic.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/clamp.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include <iostream>
#include <fcppt/math/vector/output.hpp>
#include <fcppt/math/vector/length.hpp>

namespace flake
{
void
advect(
	flake::boundary_type::type const b,
	flake::const_scalar_view const &source,
	flake::scalar_view const &destination,
	flake::vector2_grid const &velocity_field,
	flake::scalar_duration const &diff)
{
	/*
	flake::scalar const dt0 =
		static_cast<flake::scalar>(
			source.dim()[0]-1) *
		diff.count();
	*/
	flake::scalar const dt0 = diff.count();

	typedef
	flake::mizuiro_dimension_to_fcppt_vector_type<flake::scalar_view::dim_type>::type
	integer_vector_type;

	typedef
	flake::vector2_grid::value_type
	float_vector_type;

	FCPPT_ASSERT_PRE(
		velocity_field.size() ==
		flake::mizuiro_dimension_to_fcppt_dim(
			source.dim()));

	for(
		flake::const_scalar_view::iterator current_pixel =
			source.begin();
		current_pixel != source.end();
		++current_pixel)
	{
		flake::scalar_view::dim_type const current_position =
			mizuiro::image::iterator_position(
				source,
				current_pixel);

		integer_vector_type const current_position_vector =
			flake::mizuiro_dimension_to_fcppt_vector(
				current_position);

		if(
			current_position_vector[0] == 0 ||
			current_position_vector[1] == 0 ||
			current_position_vector[0] == (source.dim()[0]-1) ||
			current_position_vector[1] == (source.dim()[1]-1))
			continue;

		float_vector_type backtracked_position =
			fcppt::math::vector::structure_cast<float_vector_type>(
				current_position_vector) -
			dt0 *
			velocity_field[
				fcppt::math::vector::structure_cast<flake::vector2_grid::dim>(
					current_position_vector)];

//		std::cout << "current position: " << current_position_vector << ", backtracked: " << backtracked_position << " (arrow length " << fcppt::math::vector::length(velocity_field[
//				fcppt::math::vector::structure_cast<flake::vector2_grid::dim>(
//					current_position_vector)]) << ")\n";

		for(
			float_vector_type::iterator i =
				backtracked_position.begin();
			i != backtracked_position.end();
			++i)
			*i =
				fcppt::math::clamp(
					*i,
					static_cast<flake::scalar>(
						0.5f),
					static_cast<flake::scalar>(
						source.dim()[0]-1) +
					0.5f);

		integer_vector_type const
			backtracked_position_integral =
				fcppt::math::vector::structure_cast<integer_vector_type>(
					backtracked_position);

		integer_vector_type right_bottom_of_backtracked(
			backtracked_position_integral);
		for(
			integer_vector_type::iterator i =
				right_bottom_of_backtracked.begin();
			i != right_bottom_of_backtracked.end();
			++i)
			*i = (*i) + 1;

		// TODO: This is 2-dimensional interpolation, abstract for n dimensions?
		float_vector_type s,t;
		s[1] = backtracked_position[0] - static_cast<flake::scalar>(backtracked_position_integral[0]);
		s[0] = 1.0f - s[1];
		t[1] = backtracked_position[1] - static_cast<flake::scalar>(backtracked_position_integral[1]);
		t[0] = 1.0f - t[1];

		destination[current_position].set(
			mizuiro::color::channel::luminance(),
			s[0] *
			(t[0] *
			source[
				flake::scalar_view::dim_type(
					backtracked_position_integral[0],
					backtracked_position_integral[1])].get(mizuiro::color::channel::luminance()) +
			t[1] *
			source[
				flake::scalar_view::dim_type(
					backtracked_position_integral[0],
					right_bottom_of_backtracked[1])].get(mizuiro::color::channel::luminance())) +
			s[1] *
			(t[0] *
			source[
				flake::scalar_view::dim_type(
					right_bottom_of_backtracked[0],
					backtracked_position_integral[1])].get(mizuiro::color::channel::luminance()) +
			t[1] *
			source[
				flake::scalar_view::dim_type(
					right_bottom_of_backtracked[0],
					right_bottom_of_backtracked[1])].get(mizuiro::color::channel::luminance())));
	}

	flake::update_boundary(
		destination,
		b);
}
}

#endif
