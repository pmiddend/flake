#ifndef FLAKE_SCALAR_DURATION_HPP_INCLUDED
#define FLAKE_SCALAR_DURATION_HPP_INCLUDED

#include "scalar.hpp"
#include <fcppt/chrono/duration_fwd.hpp>

namespace flake
{
typedef
fcppt::chrono::duration<flake::scalar>
scalar_duration;
}

#endif
