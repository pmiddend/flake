#ifndef FLAKELIB_ADDITIONAL_PLANAR_DATA_HPP_INCLUDED
#define FLAKELIB_ADDITIONAL_PLANAR_DATA_HPP_INCLUDED

#include <flakelib/association_list/object.hpp>
#include <flakelib/buffer/planar_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/string.hpp>


namespace flakelib
{
typedef
flakelib::association_list::object
<
	fcppt::string,
	flakelib::buffer::planar_view<cl_float> const *
>
additional_planar_data;
}

#endif
