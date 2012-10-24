#ifndef FLAKELIB_CONSOLE_PERCENT_BAR_HPP_INCLUDED
#define FLAKELIB_CONSOLE_PERCENT_BAR_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
class console_percent_bar
{
FCPPT_NONCOPYABLE(
	console_percent_bar);
public:
	typedef
	float
	numeric_type;

	FCPPT_MAKE_STRONG_TYPEDEF(
		numeric_type,
		maximum);

	FLAKELIB_SYMBOL
	explicit
	console_percent_bar(
		maximum const &);

	FLAKELIB_SYMBOL
	void
	update(
		numeric_type);

	FLAKELIB_SYMBOL
	~console_percent_bar();
private:
	maximum const maximum_;
	unsigned terminal_columns_;
};
}

#endif

