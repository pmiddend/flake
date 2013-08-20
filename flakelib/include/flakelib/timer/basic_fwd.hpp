#ifndef FLAKELIB_TIMER_BASIC_FWD_HPP_INCLUDED
#define FLAKELIB_TIMER_BASIC_FWD_HPP_INCLUDED

#include <fcppt/config/external_begin.hpp>
#include <chrono>
#include <string>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace timer
{
template
<
	typename Char,
	typename CharTraits = std::char_traits<Char>,
//	typename ClockType = std::chrono::process_real_cpu_clock
	typename ClockType = std::chrono::high_resolution_clock
>
class basic;
}
}

#endif
