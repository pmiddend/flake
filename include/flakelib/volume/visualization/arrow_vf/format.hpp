#ifndef FLAKELIB_VOLUME_VISUALIZATION_ARROW_VF_FORMAT_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_ARROW_VF_FORMAT_HPP_INCLUDED

#include <flakelib/volume/visualization/arrow_vf/format_part.hpp>
#include <sge/renderer/vf/format.hpp>
#include <boost/mpl/vector/vector10.hpp>

namespace flakelib
{
namespace volume
{
namespace visualization
{
namespace arrow_vf
{
typedef
sge::renderer::vf::format
<
	boost::mpl::vector1<arrow_vf::format_part>
>
format;
}
}
}
}

#endif
