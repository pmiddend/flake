#ifndef FLAKELIB_VOLUME_UNIQUE_FLOAT4_BUFFER_LOCK_HPP_INCLUDED
#define FLAKELIB_VOLUME_UNIQUE_FLOAT4_BUFFER_LOCK_HPP_INCLUDED

#include <flakelib/volume/float4_buffer_lock.hpp>
#include <fcppt/config/external_begin.hpp>
#include <memory>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace volume
{
typedef
std::unique_ptr<volume::float4_buffer_lock>
unique_float4_buffer_lock;
}
}

#endif

