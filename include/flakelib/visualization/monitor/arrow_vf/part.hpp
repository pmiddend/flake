#ifndef FLAKELIB_VISUALIZATION_MONITOR_ARROW_VF_PART_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_ARROW_VF_PART_HPP_INCLUDED

#include <flakelib/visualization/monitor/arrow_vf/color.hpp>
#include <flakelib/visualization/monitor/arrow_vf/position.hpp>
#include <sge/renderer/vf/part.hpp>
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
sge::renderer::vf::part
<
	boost::mpl::vector2<arrow_vf::position,arrow_vf::color>
>
part;
}
}
}
}

#endif
