#ifndef FLAKELIB_MAP_KEY_SEQUENCE_HPP_INCLUDED
#define FLAKELIB_MAP_KEY_SEQUENCE_HPP_INCLUDED

#include <fcppt/config/external_begin.hpp>
#include <boost/function.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/core.hpp>
#include <map>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
template<typename Map>
boost::iterator_range
<
	boost::transform_iterator
	<
		boost::function
		<
			typename Map::key_type(
				typename Map::value_type const &)
		>,
		typename Map::const_iterator
	>
> const
map_key_sequence(
	Map const &m)
{
	typedef
	boost::transform_iterator
	<
		boost::function
		<
			typename Map::key_type(
				typename Map::value_type const &)
		>,
		typename Map::const_iterator
	>
	iterator_type;

	return
		boost::make_iterator_range(
			iterator_type(
				m.begin(),
				boost::phoenix::bind(
					&Map::value_type::first,
					boost::phoenix::arg_names::arg1)),
			iterator_type(
				m.end(),
				boost::phoenix::bind(
					&Map::value_type::first,
					boost::phoenix::arg_names::arg1)));
}
}

#endif

