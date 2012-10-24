#ifndef FLAKELIB_PLANAR_FLOAT2_VIEW_HPP_INCLUDED
#define FLAKELIB_PLANAR_FLOAT2_VIEW_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace planar
{
typedef
flakelib::buffer::planar_view<cl_float2>
float2_view;
}
}

#endif

