#ifndef FLAKE_MIZUIRO_DIMENSION_TO_FCPPT_VECTOR_HPP_INCLUDED
#define FLAKE_MIZUIRO_DIMENSION_TO_FCPPT_VECTOR_HPP_INCLUDED

#include "mizuiro_dimension_to_fcppt_vector_type.hpp"
#include <fcppt/math/vector/basic_impl.hpp>
#include <mizuiro/image/dimension.hpp>
#include <mizuiro/size_type.hpp>

namespace flake
{
template<typename T,mizuiro::size_type N>
typename
flake::mizuiro_dimension_to_fcppt_vector_type
<
	mizuiro::image::dimension<N,T>
>::type const
mizuiro_dimension_to_fcppt_vector(
	mizuiro::image::dimension<N,T> const &d)
{
	typedef typename
	flake::mizuiro_dimension_to_fcppt_vector_type
	<
		mizuiro::image::dimension<N,T>
	>::type
	result_type;

	result_type result;

	for(mizuiro::size_type i = 0; i < N; ++i)
		result[static_cast<typename result_type::size_type>(i)] = d[i];

	return result;
}
}

#endif
