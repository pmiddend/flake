#ifndef FLAKELIB_VISUALIZATION_ARROW_VF_PART_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_ARROW_VF_PART_HPP_INCLUDED

#include "position.hpp"
#include "color.hpp"
#include <sge/renderer/vf/part.hpp>
#include <boost/mpl/vector/vector10.hpp>

namespace flakelib
{
namespace visualization
{
namespace arrow_vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector2<arrow_vf::position,arrow_vf::color>
>
part;
}
}
}

#endif
