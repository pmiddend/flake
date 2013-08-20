#ifndef FLAKELIB_TIMER_BASIC_IMPL_HPP_INCLUDED
#define FLAKELIB_TIMER_BASIC_IMPL_HPP_INCLUDED

#include <flakelib/timer/basic_decl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <chrono>
#include <fcppt/config/external_end.hpp>


template
<
	typename Char,
	typename CharTraits,
	typename Clock
>
flakelib::timer::basic<Char,CharTraits,Clock>::basic(
	stream_type &_stream,
	string_type const &_description)
:
	stream_(
		_stream),
	description_(
		_description),
	start_(
		clock_type::now())
{
}

template
<
	typename Char,
	typename CharTraits,
	typename Clock
>
flakelib::timer::basic<Char,CharTraits,Clock>::~basic()
{
	time_point_type const now =
		clock_type::now();

	stream_
		<< description_
		<< stream_.widen(':')
		<< stream_.widen(' ')
		<<
			std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count()
		<< stream_.widen('m')
		<< stream_.widen('s')
		<< stream_.widen('\n');
}

#endif
