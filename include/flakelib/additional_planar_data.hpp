#ifndef FLAKELIB_ADDITIONAL_PLANAR_DATA_HPP_INCLUDED
#define FLAKELIB_ADDITIONAL_PLANAR_DATA_HPP_INCLUDED

#include <flakelib/planar_object.hpp>
#include <flakelib/association_list/object.hpp>
#include <fcppt/string.hpp>


namespace flakelib
{
typedef
flakelib::association_list::object
<
	fcppt::string,
	flakelib::planar_object
>
additional_planar_data;
}

#endif
