#ifndef FLAKELIB_PLANAR_MONITOR_CHILD_LIST_HPP_INCLUDED
#define FLAKELIB_PLANAR_MONITOR_CHILD_LIST_HPP_INCLUDED

#include <flakelib/planar/monitor/child.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
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
