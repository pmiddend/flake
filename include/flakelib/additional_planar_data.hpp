#ifndef FLAKELIB_ADDITIONAL_PLANAR_DATA_HPP_INCLUDED
#define FLAKELIB_ADDITIONAL_PLANAR_DATA_HPP_INCLUDED

#include <flakelib/planar_object.hpp>
#include <fcppt/string.hpp>
#include <fcppt/config/external_begin.hpp>
#include <vector>
#include <utility>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
typedef
std::vector
<
	std::pair
	<
		fcppt::string,
		flakelib::planar_object
	>
>
additional_planar_data;
}

#endif
