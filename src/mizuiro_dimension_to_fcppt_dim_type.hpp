#ifndef FLAKE_MIZUIRO_DIMENSION_TO_FCPPT_DIM_TYPE_HPP_INCLUDED
#define FLAKE_MIZUIRO_DIMENSION_TO_FCPPT_DIM_TYPE_HPP_INCLUDED

#include <mizuiro/image/dimension.hpp>
#include <mizuiro/size_type.hpp>
#include <fcppt/math/size_type.hpp>
#include <fcppt/math/dim/static.hpp>

namespace flake
{
template<typename MizuiroDim>
struct mizuiro_dimension_to_fcppt_dim_type
{
};

template<typename T,mizuiro::size_type N>
struct mizuiro_dimension_to_fcppt_dim_type
<
	mizuiro::image::dimension<N,T>
>
{
	typedef typename
	fcppt::math::dim::static_
	<
		T,
		static_cast<fcppt::math::size_type>(
			N)
	>::type
	type;
};
}

#endif
