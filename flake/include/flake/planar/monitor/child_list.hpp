#ifndef FLAKE_PLANAR_MONITOR_CHILD_LIST_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_CHILD_LIST_HPP_INCLUDED

#include <flake/planar/monitor/child.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>

namespace flake
{
namespace planar
{
namespace monitor
{
typedef
boost::intrusive::list
<
	monitor::child,
	boost::intrusive::constant_time_size<false>
>
child_list;
}
}
}

#endif
