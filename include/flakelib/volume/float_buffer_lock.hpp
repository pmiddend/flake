#ifndef FLAKELIB_VOLUME_FLOAT_BUFFER_LOCK_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLOAT_BUFFER_LOCK_HPP_INCLUDED

#include <flakelib/buffer_pool/volume_lock_fwd.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace volume
{
typedef
flakelib::buffer_pool::volume_lock<cl_float>
float_buffer_lock;
}
}

#endif

