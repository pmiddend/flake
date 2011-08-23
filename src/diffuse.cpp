#include "diffuse.hpp"
#include "update_boundary.hpp"
#include "boundary_type.hpp"
#include <mizuiro/color/channel/luminance.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/clamp.hpp>

namespace
{
// TODO: Abstract so it works in any dimension (use binary vectors)
flake::scalar
sum_von_neumann(
	flake::const_scalar_view const &v,
	flake::const_scalar_view::dim_type const &p)
{
	typedef
	flake::const_scalar_view::dim_type
	dim_type;

	FCPPT_ASSERT_PRE(
		p[0] != 0 && p[0] < static_cast<dim_type::value_type>(v.dim()[0]-1));

	FCPPT_ASSERT_PRE(
		p[1] != 0 && p[1] < static_cast<dim_type::value_type>(v.dim()[1]-1));

	return
		v[dim_type(p[0]-1u,p[1])].get(
			mizuiro::color::channel::luminance()) +
		v[dim_type(p[0]+1u,p[1])].get(
			mizuiro::color::channel::luminance()) +
		v[dim_type(p[0],p[1]-1u)].get(
			mizuiro::color::channel::luminance()) +
		v[dim_type(p[0],p[1]+1u)].get(
			mizuiro::color::channel::luminance());
}

void
diffuse_iteration_pixel(
	flake::const_scalar_view const &source,
	flake::scalar_view const &destination,
	flake::scalar const a,
	flake::scalar_view::dim_type const &p)
{
	destination[p].set(
		mizuiro::color::channel::luminance(),
		fcppt::math::clamp(
			(
				source[p].get(
					mizuiro::color::channel::luminance()) +
				a *
				sum_von_neumann(
					source,
					p)
			)
			/
			( 1.0+4.0*a ),
			flake::scalar(
				0.0f),
			flake::scalar(
				1.0f)));
}

void
diffuse_iteration(
	flake::const_scalar_view const &source,
	flake::scalar_view const &destination,
	flake::scalar const a)
{
	/*
	for(
		flake::const_scalar_view::iterator source_it =
			source.begin();
		source_it != source.end();
		++source_it)
	{
		
	}
	*/
	for(flake::scalar_view::dim_type::size_type x = 1; x < static_cast<flake::scalar_view::dim_type::size_type>(source.dim()[0]-1); ++x)
	{
		for(flake::scalar_view::dim_type::size_type y = 1; y < static_cast<flake::scalar_view::dim_type::size_type>(source.dim()[1]-1); ++y)
		{
			diffuse_iteration_pixel(
				source,
				destination,
				a,
				flake::scalar_view::dim_type(
					x,
					y));
		}
	}
}
}

void
flake::diffuse(
	flake::const_scalar_view const &source,
	flake::scalar_view const &destination,
	flake::diffusion_coefficient const &diffcoeff,
	flake::scalar_duration const &timediff)
{
	FCPPT_ASSERT_PRE(
		source.dim() == destination.dim());
	flake::scalar const a =
		timediff.count() *
		diffcoeff.get() *
		static_cast<flake::scalar>(
			source.dim()[0] * source.dim()[1]);

	// Arbitrary? o_O
	unsigned const iterations = 20;

	for(unsigned k = 0; k < iterations; ++k)
		diffuse_iteration(
			source,
			destination,
			a);

	flake::update_boundary(
		destination,
		flake::boundary_type::horizontal);
}
