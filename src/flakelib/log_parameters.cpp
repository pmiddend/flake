#include <flakelib/log_context.hpp>
#include <flakelib/log_parameters.hpp>
#include <fcppt/io/clog.hpp>
#include <fcppt/log/level.hpp>
#include <fcppt/log/location_fwd.hpp>
#include <fcppt/log/parameters/object.hpp>
#include <fcppt/log/parameters/with_context.hpp>


fcppt::log::parameters::object
flakelib::log_parameters(
	fcppt::log::location const &_location)
{
	return
		fcppt::log::parameters::with_context(
			flakelib::log_context(),
			_location)
		.level_defaults(
			fcppt::io::clog(),
			fcppt::log::level::warning)
		.enabled(
			true);

}
