#ifndef FLAKE_DURATION_HPP_INCLUDED
#define FLAKE_DURATION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <fcppt/chrono/duration_fwd.hpp>

namespace flake
{
typedef
fcppt::chrono::duration<sge::renderer::scalar>
duration;
}

#endif
