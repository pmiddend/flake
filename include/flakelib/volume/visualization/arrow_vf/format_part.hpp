#ifndef FLAKELIB_VOLUME_VISUALIZATION_ARROW_VF_FORMAT_PART_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_ARROW_VF_FORMAT_PART_HPP_INCLUDED

#include <flakelib/volume/visualization/arrow_vf/color.hpp>
#include <flakelib/volume/visualization/arrow_vf/position.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace volume
{
namespace visualization
{
namespace arrow_vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector2
	<
		arrow_vf::position,
		arrow_vf::color
	>
>
format_part;
}
}
}
}

#endif
