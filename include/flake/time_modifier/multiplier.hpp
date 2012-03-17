#ifndef FLAKE_TIME_MODIFIER_MULTIPLIER_HPP_INCLUDED
#define FLAKE_TIME_MODIFIER_MULTIPLIER_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/rational.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace time_modifier
{
FCPPT_MAKE_STRONG_TYPEDEF(
	boost::rational<int>,
	multiplier);
}
}

#endif

