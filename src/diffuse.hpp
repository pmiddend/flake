#ifndef FLAKE_DIFFUSE_HPP_INCLUDED
#define FLAKE_DIFFUSE_HPP_INCLUDED

#include "ndim/clamp.hpp"
#include "ndim/sum_von_neumann.hpp"
#include "ndim/on_border.hpp"
#include "diffusion_coefficient.hpp"
#include "update_boundary.hpp"
#include "boundary_type.hpp"
#include "scalar.hpp"
#include "has_value_type.hpp"
#include "scalar_duration.hpp"
#include <fcppt/container/grid/object_impl.hpp>
#include <fcppt/container/grid/iterator_position.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/type_traits/is_same.hpp>

namespace flake
{
namespace detail
{
template<typename Grid>
void
diffuse_iteration(
	Grid const &source,
	Grid &destination,
	flake::scalar const a)
{
	typedef typename
	Grid::const_iterator
	const_grid_iterator;

	typedef typename
	Grid::dim
	dim;

	for(
		const_grid_iterator source_it =
			source.begin();
		source_it != source.end();
		++source_it)
	{
		dim const current_pixel =
			fcppt::container::grid::iterator_position(
				source,
				source_it);

		if(
			ndim::on_border(
				source,
				current_pixel))
			continue;

		flake::scalar const denominator =
			1.0f +
			static_cast<flake::scalar>(
				1 << Grid::dim::dim_wrapper::value) *
			a;

		destination[current_pixel] =
			ndim::clamp(
				(
				source[current_pixel] +
				a *
				ndim::sum_von_neumann(
					source,
					current_pixel)
				)
				/ denominator,
				flake::scalar(0.0f),
				flake::scalar(1.0f));
	}
}
}

namespace detail
{
// Hack for the eval_if below
template<typename T>
struct value_type_to_type
{
	typedef typename
	T::value_type
	type;
};
}

template<typename Grid>
void
diffuse(
	flake::boundary_type::type const b,
	Grid const &source,
	Grid &destination,
	flake::diffusion_coefficient const &diff,
	flake::scalar_duration const &timediff)
{
	FCPPT_ASSERT_PRE(
		source.size() == destination.size());

	typedef typename
	Grid::value_type
	vector_type;

	typedef typename
	boost::mpl::eval_if
	<
		flake::has_value_type<vector_type>,
		detail::value_type_to_type<vector_type>,
		boost::mpl::identity<flake::scalar>
	>::type
	scalar_type;

	flake::scalar const a =
		timediff.count() *
		diff.get() *
		static_cast<flake::scalar>(
			source.size().content());

	// Arbitrary? o_O
	unsigned const iterations = 20;

	for(unsigned k = 0; k < iterations; ++k)
		detail::diffuse_iteration(
			source,
			destination,
			a);

	flake::update_boundary(
		destination,
		b);
}
}

#endif
