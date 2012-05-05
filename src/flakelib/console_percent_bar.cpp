#include <iomanip>
#include <fcppt/text.hpp>
#include <flakelib/console_percent_bar.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cmath>
#include <iostream>
#include <fcppt/config/external_end.hpp>
#include <fcppt/config/platform.hpp>
#ifdef FCPPT_CONFIG_UNIX_PLATFORM
#include <fcppt/config/external_begin.hpp>
#include <termcap.h>
#include <cstdlib>
#include <fcppt/config/external_end.hpp>
#endif

flakelib::console_percent_bar::console_percent_bar(
	maximum const &_maximum)
:
	maximum_(
		_maximum),
	terminal_columns_()
{
	unsigned const default_columns = 80u;

#ifdef FCPPT_CONFIG_UNIX_PLATFORM
	char termbuf[2048];

	if(tgetent(termbuf,std::getenv("TERM")) == -1)
	{
		terminal_columns_ = default_columns;
	}
	else
	{
		int const columns =
			tgetnum("co");

		if(columns == -1)
			terminal_columns_ =
				default_columns;
		else
			terminal_columns_ =
				static_cast<unsigned>(
					columns - 10);
	}
#else
	terminal_columns_ =
		default_columns;
#endif

	this->update(
		static_cast<numeric_type>(
			0));
}

void
flakelib::console_percent_bar::update(
	numeric_type const _value)
{
	std::cout << "\r" << std::flush << "├";

	numeric_type percent_value =
		std::floor(
			_value / maximum_.get() * 100.0f + 0.5f);

	unsigned const filled_characters =
		static_cast<unsigned>(
			_value / maximum_.get() *
			static_cast<numeric_type>(
				terminal_columns_));

	for(
		unsigned i = 0;
		i < filled_characters;
		++i)
		std::cout << "─";

	std::cout
		<<
			std::string(
				static_cast<std::string::size_type>(
					terminal_columns_ - filled_characters),
				' ');

	std::cout << "┤ " << std::setw(3) << static_cast<unsigned>(percent_value) << "%";
}

flakelib::console_percent_bar::~console_percent_bar()
{
	this->update(
		maximum_.get());
}
