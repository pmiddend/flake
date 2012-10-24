#ifndef FLAKELIB_VOLUME_FLOAT4_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLOAT4_VIEW_HPP_INCLUDED

#include <flakelib/buffer/volume_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace volume
{
typedef
flakelib::buffer::volume_view<cl_float4>
float4_view;
}
}

#endif

