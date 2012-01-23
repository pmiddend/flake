#ifndef FLAKELIB_VOLUME_FLAKES_VF_POSITION_PART_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_VF_POSITION_PART_HPP_INCLUDED

#include <flakelib/volume/flakes/vf/position.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace volume
{
namespace flakes
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
position_part;
}
}
}
}

#endif
