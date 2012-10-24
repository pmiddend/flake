#include <flakelib/exception.hpp>
#include <fcppt/assert/make_message.hpp>

flakelib::exception::exception(
	fcppt::string const &_string)
:
	fcppt::exception(
		_string)
{
}

flakelib::exception::exception(
	fcppt::assert_::information const &_info)
:
	fcppt::exception(
		fcppt::assert_::make_message(
			_info))
{
}

flakelib::exception::~exception() throw()
{
}
