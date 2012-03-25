#ifndef FLAKELIB_DURATION_HPP_INCLUDED
#define FLAKELIB_DURATION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <boost/chrono/duration.hpp>

namespace flakelib
{
typedef
boost::chrono::duration<sge::renderer::scalar>
duration;
}

#endif
