#ifndef FLAKELIB_LINEAR_UINT_VIEW_HPP_INCLUDED
#define FLAKELIB_LINEAR_UINT_VIEW_HPP_INCLUDED

#include <flakelib/buffer/linear_view_fwd.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace linear
{
typedef
flakelib::buffer::linear_view<cl_uint>
uint_view;
}
}

#endif

