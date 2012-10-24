#ifndef FLAKE_PLANAR_MONITOR_ARROW_VF_FORMAT_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_ARROW_VF_FORMAT_HPP_INCLUDED

#include <flake/planar/monitor/arrow_vf/part.hpp>
#include <sge/renderer/vf/format.hpp>
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
