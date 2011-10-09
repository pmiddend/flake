#ifndef FLAKELIB_DURATION_HPP_INCLUDED
#define FLAKELIB_DURATION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <fcppt/chrono/duration_fwd.hpp>

namespace flakelib
{
typedef
fcppt::chrono::duration<sge::renderer::scalar>
duration;
}

#endif
