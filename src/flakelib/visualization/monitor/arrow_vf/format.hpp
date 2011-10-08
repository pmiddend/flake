#ifndef FLAKELIB_VISUALIZATION_MONITOR_ARROW_VF_FORMAT_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_ARROW_VF_FORMAT_HPP_INCLUDED

#include <flakelib/visualization/monitor/arrow_vf/part.hpp>
#include <sge/renderer/vf/format.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace visualization
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
}

#endif
