#ifndef FLAKELIB_MONITOR_ARROW_VF_FORMAT_HPP_INCLUDED
#define FLAKELIB_MONITOR_ARROW_VF_FORMAT_HPP_INCLUDED

#include <flakelib/monitor/arrow_vf/part.hpp>
#include <sge/renderer/vf/format.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace monitor
{
namespace arrow_vf
{
typedef
sge::renderer::vf::format
<
	boost::mpl::vector1<arrow_vf::part>
>
format;
}
}
}

#endif
