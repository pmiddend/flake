#ifndef FLAKE_NDIM_CLAMP_HPP_INCLUDED
#define FLAKE_NDIM_CLAMP_HPP_INCLUDED

#include <fcppt/math/clamp.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_floating_point.hpp>

namespace flake
{
namespace ndim
{
template<typename T>
typename
boost::enable_if
<
	boost::is_floating_point<T>,
	T
>::type
clamp(
	T const input,
	T const dmin,
	T const dmax)
{
	return
		fcppt::math::clamp(
			input,
			dmin,
			dmax);
}

template<typename Vector>
typename
boost::disable_if
<
	boost::is_floating_point<Vector>,
	Vector
>::type
clamp(
	Vector const input,
	typename Vector::value_type const dmin,
	typename Vector::value_type const dmax)
{
	typedef typename
	Vector::size_type
	vector_size_type;

	Vector result;

	for(
		vector_size_type i =
			0;
		i < input.size();
		++i)
		result[i] =
			fcppt::math::clamp(
				input[i],
				dmin,
				dmax);

	return result;
}
}
}

#endif
