#ifndef FLAKELIB_VOLUME_VISUALIZATION_ARROW_VF_FORMAT_PART_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_ARROW_VF_FORMAT_PART_VIEW_HPP_INCLUDED

#include <flakelib/volume/visualization/arrow_vf/format_part.hpp>
#include <sge/renderer/vf/view.hpp>

namespace flakelib
{
namespace volume
{
namespace visualization
{
namespace arrow_vf
{
typedef
sge::renderer::vf::view<arrow_vf::format_part>
format_part_view;
}
}
}
}

#endif
