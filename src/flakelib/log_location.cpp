#include <flakelib/log_location.hpp>
#include <fcppt/log/location.hpp>
#include <fcppt/text.hpp>

fcppt::log::location const
flakelib::log_location()
{
	return
		fcppt::log::location(
			FCPPT_TEXT("flakelib"));
}
