#ifndef FLAKELIB_PLANAR_POOL_UNIQUE_LOCK_HPP_INCLUDED
#define FLAKELIB_PLANAR_POOL_UNIQUE_LOCK_HPP_INCLUDED

#include <flakelib/planar_pool/scoped_lock_fwd.hpp>
#include <fcppt/unique_ptr.hpp>

namespace flakelib
{
namespace planar_pool
{
typedef
fcppt::unique_ptr<planar_pool::scoped_lock>
unique_lock;
}
}

#endif
