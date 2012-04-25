#ifndef FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_VF_FORMAT_PART_HPP_INCLUDED
#define FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_VF_FORMAT_PART_HPP_INCLUDED

#include <flake/volume/density_visualization/raycaster/vf/position.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace volume
{
namespace density_visualization
{
namespace raycaster
{
namespace vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector1
	<
		vf::position
	>
>
format_part;
}
}
}
}
}

#endif

