#ifndef FLAKELIB_VOLUME_DENSITY_CUBE_VF_FORMAT_PART_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_DENSITY_CUBE_VF_FORMAT_PART_VIEW_HPP_INCLUDED

#include <flakelib/volume/density/cube_vf/format_part.hpp>
#include <sge/renderer/vf/view.hpp>

namespace flakelib
{
namespace volume
{
namespace density
{
namespace cube_vf
{
typedef
sge::renderer::vf::view<cube_vf::format_part>
format_part_view;
}
}
}
}

#endif
