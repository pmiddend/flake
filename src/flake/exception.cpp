#include <flake/exception.hpp>
#include <fcppt/assert/make_message.hpp>

flake::exception::exception(
	fcppt::string const &_string)
:
	fcppt::exception(
		_string)
{
}

flake::exception::exception(
	fcppt::assert_::information const &_info)
:
	fcppt::exception(
		fcppt::assert_::make_message(
			_info))
{
}

flake::exception::~exception() throw()
{
}
