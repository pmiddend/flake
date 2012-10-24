#ifndef FLAKELIB_LOG_PARAMETERS_HPP_INCLUDED
#define FLAKELIB_LOG_PARAMETERS_HPP_INCLUDED

#include <fcppt/log/location_fwd.hpp>
#include <fcppt/log/parameters/object.hpp>

namespace flakelib
{
fcppt::log::parameters::object
log_parameters(
	fcppt::log::location const &);
}

#endif

