#ifndef FLAKELIB_PLANAR_FLOAT_BUFFER_LOCK_HPP_INCLUDED
#define FLAKELIB_PLANAR_FLOAT_BUFFER_LOCK_HPP_INCLUDED

#include <flakelib/buffer_pool/planar_lock_fwd.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace planar
{
typedef
flakelib::buffer_pool::planar_lock<cl_float>
float_buffer_lock;
}
}

#endif

