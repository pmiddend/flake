#ifndef FLAKELIB_VOLUME_UNIQUE_FLOAT4_BUFFER_LOCK_HPP_INCLUDED
#define FLAKELIB_VOLUME_UNIQUE_FLOAT4_BUFFER_LOCK_HPP_INCLUDED

#include <flakelib/volume/float4_buffer_lock.hpp>
#include <fcppt/unique_ptr_impl.hpp>

namespace flakelib
{
namespace volume
{
typedef
fcppt::unique_ptr<volume::float4_buffer_lock>
unique_float4_buffer_lock;
}
}

#endif

