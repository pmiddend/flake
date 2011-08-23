#ifndef FLAKE_MIZUIRO_DIMENSION_TO_FCPPT_VECTOR_TYPE_HPP_INCLUDED
#define FLAKE_MIZUIRO_DIMENSION_TO_FCPPT_VECTOR_TYPE_HPP_INCLUDED

#include <mizuiro/image/dimension.hpp>
#include <mizuiro/size_type.hpp>
#include <fcppt/math/size_type.hpp>
#include <fcppt/math/vector/static.hpp>

namespace flake
{
template<typename MizuiroDim>
struct mizuiro_dimension_to_fcppt_vector_type
{
};

template<typename T,mizuiro::size_type N>
struct mizuiro_dimension_to_fcppt_vector_type
<
	mizuiro::image::dimension<N,T>
>
{
	typedef typename
	fcppt::math::vector::static_
	<
		T,
		static_cast<fcppt::math::size_type>(
			N)
	>::type
	type;
};
}

#endif
