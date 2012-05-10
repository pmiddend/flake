#ifndef FLAKELIB_VOLUME_UNIQUE_UINT_BUFFER_LOCK_HPP_INCLUDED
#define FLAKELIB_VOLUME_UNIQUE_UINT_BUFFER_LOCK_HPP_INCLUDED

#include <flakelib/buffer_pool/volume_lock_fwd.hpp>
#include <flakelib/volume/uint_buffer_lock.hpp>
#include <fcppt/unique_ptr.hpp>

namespace flakelib
{
namespace volume
{
typedef
fcppt::unique_ptr<flakelib::volume::uint_buffer_lock>
unique_uint_buffer_lock;
}
}

#endif

