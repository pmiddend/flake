#ifndef FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_FORMAT_PART_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_FORMAT_PART_HPP_INCLUDED

#include <sge/renderer/vf/part.hpp>
#include <flakelib/volume/visualization/model_vf/position.hpp>
#include <flakelib/volume/visualization/model_vf/texcoord.hpp>
#include <flakelib/volume/visualization/model_vf/normal.hpp>
#include <boost/mpl/vector/vector10.hpp>

namespace flakelib
{
namespace volume
{
namespace visualization
{
namespace model_vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector3
	<
		model_vf::position,
		model_vf::texcoord,
		model_vf::normal
	>
>
format_part;
}
}
}
}

#endif
