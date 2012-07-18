#ifndef FLAKELIB_TIMER_BASIC_DECL_HPP_INCLUDED
#define FLAKELIB_TIMER_BASIC_DECL_HPP_INCLUDED

#include <flakelib/timer/basic_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <string>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace timer
{
template
<
	typename Char,
	typename CharTraits,
	typename Clock
>
class basic
{
FCPPT_NONCOPYABLE(
	basic);
public:
	typedef
	Char
	char_type;

	typedef
	CharTraits
	char_traits_type;

	typedef
	Clock
	clock_type;

	typedef
	std::basic_string<char_type,char_traits_type>
	string_type;

	typedef
	std::basic_ostream<char_type,char_traits_type>
	stream_type;

	basic(
		stream_type &,
		string_type const &);

	~basic();
private:
	typedef typename
	clock_type::time_point
	time_point_type;

	stream_type &stream_;
	string_type const description_;
	time_point_type start_;
};
}
}

#endif
