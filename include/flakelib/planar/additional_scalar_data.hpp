#ifndef FLAKELIB_PLANAR_ADDITIONAL_SCALAR_DATA_HPP_INCLUDED
#define FLAKELIB_PLANAR_ADDITIONAL_SCALAR_DATA_HPP_INCLUDED

#include <flakelib/association_list/object.hpp>
#include <flakelib/buffer/planar_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <fcppt/string.hpp>

namespace flakelib
{
namespace planar
{
typedef
flakelib::association_list::object
<
	fcppt::string,
	flakelib::buffer::planar_view<cl_float> const *
>
additional_scalar_data;
}
}


#endif
