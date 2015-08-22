#ifndef FLAKELIB_VOLUME_CONVERSION_PLANAR_SIZE_FROM_VOLUME_SIZE_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_PLANAR_SIZE_FROM_VOLUME_SIZE_HPP_INCLUDED

#include <fcppt/math/is_power_of_2.hpp>
#include <fcppt/math/next_power_of_2.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/static.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/promote.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/enable_if.hpp>
#include <fcppt/config/external_end.hpp>


namespace fcppt
{
namespace math
{
template<typename Int>
typename
boost::enable_if
<
	boost::is_unsigned<Int>,
	Int
>::type
discrete_sqrt(
	Int const _input)
{
	typedef typename
	boost::promote<Int>::type
	promoted;

	promoted result = 0u;
	promoted current_uneven_number = 1u;
	Int counter = static_cast<Int>(0u);

	for(;result < _input; counter++)
	{
		result += current_uneven_number;
		current_uneven_number += 2u;
	}

	return
		counter;
}
}
}


namespace flakelib
{
namespace volume
{
namespace conversion
{
template<typename T,typename N,typename S>
typename
boost::disable_if
<
	boost::is_floating_point<T>,
	fcppt::math::dim::static_
	<
		T,
		N::value - 1u
	>
>::type const
planar_size_from_volume_size(
	fcppt::math::dim::object<T,N,S> const &input)
{
	typedef
	fcppt::math::dim::static_
	<
		T,
		N::value - 1u
	>
	result_type;

	T const
		raw_edge_size =
			fcppt::math::discrete_sqrt(
				input.content()),
		pot_edge_size =
			fcppt::math::is_power_of_2(raw_edge_size)
			?
				raw_edge_size
			:
				fcppt::math::next_power_of_2(
					raw_edge_size);

	return
		result_type(
			pot_edge_size,
			pot_edge_size);
}
}
}
}

#endif

