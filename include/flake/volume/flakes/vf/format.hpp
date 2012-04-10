#ifndef FLAKE_VOLUME_FLAKES_VF_FORMAT_HPP_INCLUDED
#define FLAKE_VOLUME_FLAKES_VF_FORMAT_HPP_INCLUDED

#include <flake/volume/flakes/vf/point_size_part.hpp>
#include <flake/volume/flakes/vf/position_part.hpp>
#include <sge/renderer/vf/format.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace volume
{
namespace flakes
{
namespace vf
{
typedef
sge::renderer::vf::format
<
	boost::mpl::vector2
	<
		vf::position_part,
		vf::point_size_part
	>
>
format;
}
}
}
}

#endif

