#include <flakelib/log.hpp>
#include <flakelib/log_location.hpp>
#include <flakelib/log_parameters.hpp>
#include <fcppt/log/location.hpp>
#include <fcppt/log/object.hpp>


namespace
{

fcppt::log::object logger(
	flakelib::log_parameters(
		flakelib::log_location()));

}

fcppt::log::object &
flakelib::log()
{
	return logger;
}
