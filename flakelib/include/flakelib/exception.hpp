#ifndef FLAKELIB_EXCEPTION_HPP_INCLUDED
#define FLAKELIB_EXCEPTION_HPP_INCLUDED

#include <fcppt/exception.hpp>
#include <fcppt/string.hpp>
#include <fcppt/assert/information_fwd.hpp>


namespace flakelib
{
class exception
:
	public fcppt::exception
{
public:
	explicit exception(
		fcppt::string const &);

	explicit exception(
		fcppt::assert_::information const &);

	virtual ~exception() throw();
};
}

#endif
