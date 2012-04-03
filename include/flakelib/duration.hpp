#ifndef FLAKELIB_DURATION_HPP_INCLUDED
#define FLAKELIB_DURATION_HPP_INCLUDED

#include <sge/renderer/scalar.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/chrono/duration.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
typedef
boost::chrono::duration<sge::renderer::scalar>
duration;
}

#endif
