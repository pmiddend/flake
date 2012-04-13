#ifndef FLAKE_VOLUME_MODEL_VF_FORMAT_PART_HPP_INCLUDED
#define FLAKE_VOLUME_MODEL_VF_FORMAT_PART_HPP_INCLUDED

#include <flake/volume/model/vf/normal.hpp>
#include <flake/volume/model/vf/position.hpp>
#include <flake/volume/model/vf/texcoord.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace volume
{
namespace model
{
namespace vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector3
	<
		vf::position,
		vf::normal,
		vf::texcoord
	>
>
format_part;
}
}
}
}

#endif

