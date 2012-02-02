#ifndef FLAKELIB_PLANAR_FLOAT_VIEW_HPP_INCLUDED
#define FLAKELIB_PLANAR_FLOAT_VIEW_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace planar
{
typedef
flakelib::buffer::planar_view<cl_float>
float_view;
}
}

#endif

