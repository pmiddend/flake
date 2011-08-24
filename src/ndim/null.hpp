#ifndef FLAKE_NDIM_NULL_HPP_INCLUDED
#define FLAKE_NDIM_NULL_HPP_INCLUDED

#include "../has_value_type.hpp"
#include <boost/utility/enable_if.hpp>

namespace flake
{
namespace ndim
{
template<typename Vector>
typename
boost::enable_if
<
	flake::has_value_type<Vector>,
	Vector
>::type
null()
{
	return Vector::null();
}

template<typename Scalar>
typename
boost::disable_if
<
	flake::has_value_type<Scalar>,
	Scalar
>::type
null()
{
	return static_cast<Scalar>(0);
}
}
}

#endif
