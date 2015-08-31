#ifndef FLAKELIB_PLANAR_UNIQUE_FLOAT2_BUFFER_LOCK_HPP_INCLUDED
#define FLAKELIB_PLANAR_UNIQUE_FLOAT2_BUFFER_LOCK_HPP_INCLUDED

#include <flakelib/planar/float2_buffer_lock.hpp>
#include <fcppt/unique_ptr_impl.hpp>

namespace flakelib
{
namespace planar
{
typedef
fcppt::unique_ptr<planar::float2_buffer_lock>
unique_float2_buffer_lock;
}
}

#endif

