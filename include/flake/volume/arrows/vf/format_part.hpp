#ifndef FLAKE_VOLUME_ARROWS_VF_FORMAT_PART_HPP_INCLUDED
#define FLAKE_VOLUME_ARROWS_VF_FORMAT_PART_HPP_INCLUDED

#include <flake/volume/arrows/vf/color.hpp>
#include <flake/volume/arrows/vf/position.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>

namespace flake
{
namespace volume
{
namespace arrows
{
namespace vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector2
	<
		vf::position,
		vf::color
	>
>
format_part;
}
}
}
}

#endif

