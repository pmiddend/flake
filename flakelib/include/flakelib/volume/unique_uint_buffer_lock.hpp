#ifndef FLAKELIB_VOLUME_UNIQUE_UINT_BUFFER_LOCK_HPP_INCLUDED
#define FLAKELIB_VOLUME_UNIQUE_UINT_BUFFER_LOCK_HPP_INCLUDED

#include <flakelib/volume/uint_buffer_lock.hpp>
#include <fcppt/config/external_begin.hpp>
#include <memory>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace volume
{
typedef
std::unique_ptr<flakelib::volume::uint_buffer_lock>
unique_uint_buffer_lock;
}
}

#endif

