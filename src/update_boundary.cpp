#include "update_boundary.hpp"
#include <fcppt/assert/pre.hpp>
#include <mizuiro/color/channel/luminance.hpp>

void
flake::update_boundary(
	flake::scalar_view const &source,
	flake::boundary_type::type const b)
{
	FCPPT_ASSERT_PRE(
		source.dim()[0] == source.dim()[1]);

	typedef
	flake::scalar_view::dim_type
	dim_type;

	dim_type::value_type const N =
		static_cast<dim_type::value_type>(
			source.dim()[0]-1);

	for(
		dim_type::value_type i =
			static_cast<dim_type::value_type>(1);
		i <= N;
		++i)
	{
		source[dim_type(0,i)].set(
			mizuiro::color::channel::luminance(),
			b == boundary_type::horizontal
			?
				-source[dim_type(1,i)].get(
					mizuiro::color::channel::luminance())
			:
				source[dim_type(1,i)].get(
					mizuiro::color::channel::luminance()));

		source[dim_type(N,i)].set(
			mizuiro::color::channel::luminance(),
			b == boundary_type::horizontal
			?
				-source[dim_type(N-1,i)].get(
					mizuiro::color::channel::luminance())
			:
				source[dim_type(N-1,i)].get(
					mizuiro::color::channel::luminance()));

		source[dim_type(i,0)].set(
			mizuiro::color::channel::luminance(),
			b == boundary_type::vertical
			?
				-source[dim_type(i,1)].get(
					mizuiro::color::channel::luminance())
			:
				source[dim_type(i,1)].get(
					mizuiro::color::channel::luminance()));

		source[dim_type(i,N)].set(
			mizuiro::color::channel::luminance(),
			b == boundary_type::vertical
			?
				-source[dim_type(i,N-1)].get(
					mizuiro::color::channel::luminance())
			:
				source[dim_type(i,N-1)].get(
					mizuiro::color::channel::luminance()));
	}

	source[dim_type(0,0)].set(
		mizuiro::color::channel::luminance(),
		(source[dim_type(1,0)].get(
					mizuiro::color::channel::luminance()) + source[dim_type(0,1)].get(
					mizuiro::color::channel::luminance()))/2.0f);
	source[dim_type(0,N)].set(
		mizuiro::color::channel::luminance(),
		(source[dim_type(1,N)].get(
					mizuiro::color::channel::luminance()) + source[dim_type(0,N-1)].get(
					mizuiro::color::channel::luminance()))/2.0f);
	source[dim_type(N,0)].set(
		mizuiro::color::channel::luminance(),
		(source[dim_type(N-1,0)].get(
					mizuiro::color::channel::luminance()) + source[dim_type(N,1)].get(
					mizuiro::color::channel::luminance()))/2.0f);
	source[dim_type(N,N)].set(
		mizuiro::color::channel::luminance(),
		(source[dim_type(N-1,N)].get(
					mizuiro::color::channel::luminance()) + source[dim_type(N,N-1)].get(
					mizuiro::color::channel::luminance()))/2.0f);
}
