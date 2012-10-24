#ifndef FLAKELIB_VOLUME_FLOAT4_BUFFER_LOCK_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLOAT4_BUFFER_LOCK_HPP_INCLUDED

#include <flakelib/buffer_pool/volume_lock_fwd.hpp>
#include <sge/opencl/clinclude.hpp>

namespace flakelib
{
namespace volume
{
typedef
flakelib::buffer_pool::volume_lock<cl_float4>
float4_buffer_lock;
}
}

#endif

