#ifndef FLAKELIB_ADDITIONAL_PLANAR_DATA_HPP_INCLUDED
#define FLAKELIB_ADDITIONAL_PLANAR_DATA_HPP_INCLUDED

#include <flakelib/planar_object.hpp>
#include <fcppt/string.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/unordered_map.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
typedef
boost::unordered_map
<
	fcppt::string,
	flakelib::planar_object
>
additional_planar_data;
}

#endif
