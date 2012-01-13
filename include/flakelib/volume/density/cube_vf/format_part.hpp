#ifndef FLAKELIB_VOLUME_DENSITY_CUBE_VF_FORMAT_PART_HPP_INCLUDED
#define FLAKELIB_VOLUME_DENSITY_CUBE_VF_FORMAT_PART_HPP_INCLUDED

#include <sge/renderer/vf/part.hpp>
#include <flakelib/volume/density/cube_vf/position.hpp>
#include <boost/mpl/vector/vector10.hpp>

namespace flakelib
{
namespace volume
{
namespace density
{
namespace cube_vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector1
	<
		cube_vf::position
	>
>
format_part;
}
}
}
}

#endif
