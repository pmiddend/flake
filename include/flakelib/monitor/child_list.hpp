#ifndef FLAKELIB_MONITOR_CHILD_LIST_HPP_INCLUDED
#define FLAKELIB_MONITOR_CHILD_LIST_HPP_INCLUDED

#include <flakelib/monitor/child.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
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

#endif
