#ifndef FLAKE_FCPPT_DIM_TO_MIZUIRO_DIMENSION_HPP_INCLUDED
#define FLAKE_FCPPT_DIM_TO_MIZUIRO_DIMENSION_HPP_INCLUDED

#include <mizuiro/image/dimension.hpp>
#include <fcppt/math/size_type.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

namespace flake
{
template<typename T,typename N,typename S>
mizuiro::image::dimension<N::value,T> const
fcppt_dim_to_mizuiro_dimension(
	fcppt::dim::dim::basic<T,N,S> const &v)
{
	mizuiro::image::dimension<N::value,T> result;

	for(mizuiro::size_type i = 0; i < N::value; ++i)
		result[i] = v[static_cast<fcppt::math::size_type>(i)];

	return result;
}
}

#endif
