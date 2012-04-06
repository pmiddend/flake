#ifndef FLAKELIB_VOLUME_FLOAT_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLOAT_VIEW_HPP_INCLUDED

#include <flakelib/buffer/volume_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace volume
{
typedef
flakelib::buffer::volume_view<cl_float>
float_view;
}
}

#endif

