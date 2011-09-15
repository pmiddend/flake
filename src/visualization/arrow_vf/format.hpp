#ifndef FLAKE_VISUALIZATION_ARROW_VF_FORMAT_HPP_INCLUDED
#define FLAKE_VISUALIZATION_ARROW_VF_FORMAT_HPP_INCLUDED

#include "part.hpp"
#include <sge/renderer/vf/format.hpp>
#include <boost/mpl/vector/vector10.hpp>

namespace flake
{
namespace visualization
{
namespace arrow_vf
{
typedef
sge::renderer::vf::format
<
	boost::mpl::vector1<arrow_vf::part>
>
format;
}
}
}

#endif
