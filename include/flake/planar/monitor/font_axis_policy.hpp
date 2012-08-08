#ifndef FLAKE_PLANAR_MONITOR_FONT_AXIS_POLICY_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_FONT_AXIS_POLICY_HPP_INCLUDED

#include <sge/rucksack/axis_policy2.hpp>
#include <sge/font/object_fwd.hpp>
#include <sge/font/text_parameters_fwd.hpp>
#include <sge/font/string.hpp>

namespace flake
{
namespace planar
{
namespace monitor
{
sge::rucksack::axis_policy2 const
font_axis_policy(
	sge::font::object &,
	sge::font::text_parameters const &,
	sge::font::string const &);
}
}
}

#endif
