#ifndef FLAKE_PLANAR_MONITOR_ARROW_VF_PART_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_ARROW_VF_PART_HPP_INCLUDED

#include <flake/planar/monitor/arrow_vf/position.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
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
