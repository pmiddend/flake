#include <flakelib/timer.hpp>
#include <fcppt/config/external_begin.hpp>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>


flakelib::timer::timer(
	fcppt::string const &_description)
:
	description_(
		_description),
	start_(
		clock::now())
{
}

flakelib::timer::~timer()
{
	std::cout
		<< boost::chrono::duration_cast<boost::chrono::milliseconds>(clock::now() - start_).count()
		<< "ms\n";
}
