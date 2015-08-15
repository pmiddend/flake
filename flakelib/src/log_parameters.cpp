#include <flakelib/log_context.hpp>
#include <flakelib/log_parameters.hpp>
#include <fcppt/io/clog.hpp>
#include <fcppt/log/level.hpp>
#include <fcppt/log/location_fwd.hpp>
#include <fcppt/log/parameters.hpp>


fcppt::log::parameters
flakelib::log_parameters(
	fcppt::log::location const &_location)
{
	return
		fcppt::log::parameters(
			fcppt::io::clog(),
			fcppt::log::level::warning
		)
		.context_location(
			flakelib::log_context(),
			_location
		)
		.enabled(
			true
		);

}
