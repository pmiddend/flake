#ifndef FLAKELIB_VOLUME_CONVERSION_PLANAR_SIZE_FROM_VOLUME_SIZE_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_PLANAR_SIZE_FROM_VOLUME_SIZE_HPP_INCLUDED

#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/enable_if.hpp>
#include <fcppt/config/external_end.hpp>


namespace fcppt
{
namespace math
{
template<typename Int>
boost::enable_if
<
	boost::is_unsigned
	<

	>
>::
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
	fcppt::math::dim::object<T,N::value-1,S>
>::type const
planar_size_from_volume_size(
	fcppt::math::dim::object<T,N,S> const &input)
{

}
}
}
}

#endif

