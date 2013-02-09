#ifndef FLAKELIB_PLANAR_UNIQUE_FLOAT_BUFFER_LOCK_HPP_INCLUDED
#define FLAKELIB_PLANAR_UNIQUE_FLOAT_BUFFER_LOCK_HPP_INCLUDED

#include <flakelib/planar/float_buffer_lock.hpp>
#include <fcppt/config/external_begin.hpp>
#include <memory>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace planar
{
typedef
std::unique_ptr<planar::float_buffer_lock>
unique_float_buffer_lock;
}
}

#endif

