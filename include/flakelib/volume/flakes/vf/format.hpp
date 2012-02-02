#ifndef FLAKELIB_VOLUME_FLAKES_VF_FORMAT_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_VF_FORMAT_HPP_INCLUDED

#include <flakelib/volume/flakes/vf/point_size_part.hpp>
#include <flakelib/volume/flakes/vf/position_part.hpp>
#include <flakelib/volume/flakes/vf/starting_position_part.hpp>
#include <sge/renderer/vf/format.hpp>
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
sge::renderer::vf::format
<
	boost::mpl::vector3
	<
		vf::position_part,
		vf::starting_position_part,
		vf::point_size_part
	>
>
format;
}
}
}
}

#endif
