#ifndef FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_FORMAT_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_FORMAT_HPP_INCLUDED

#include <sge/renderer/vf/format.hpp>
#include <flakelib/volume/visualization/model_vf/format_part.hpp>
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
sge::renderer::vf::format
<
	boost::mpl::vector1<model_vf::format_part>
>
format;
}
}
}
}

#endif
