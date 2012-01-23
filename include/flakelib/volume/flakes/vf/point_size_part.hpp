#ifndef FLAKELIB_VOLUME_FLAKES_VF_POINT_SIZE_PART_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_VF_POINT_SIZE_PART_HPP_INCLUDED

#include <flakelib/volume/flakes/vf/point_size.hpp>
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
		vf::point_size
	>
>
point_size_part;
}
}
}
}

#endif
