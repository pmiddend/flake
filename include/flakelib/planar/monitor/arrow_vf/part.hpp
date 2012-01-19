#ifndef FLAKELIB_PLANAR_MONITOR_ARROW_VF_PART_HPP_INCLUDED
#define FLAKELIB_PLANAR_MONITOR_ARROW_VF_PART_HPP_INCLUDED

#include <flakelib/planar/monitor/arrow_vf/color.hpp>
#include <flakelib/planar/monitor/arrow_vf/position.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace planar
{
namespace monitor
{
namespace arrow_vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector1<arrow_vf::position>
>
part;
}
}
}
}

#endif
