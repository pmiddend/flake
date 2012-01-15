#ifndef FLAKELIB_VOLUME_FLAKES_VF_FORMAT_PART_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_VF_FORMAT_PART_VIEW_HPP_INCLUDED

#include <flakelib/volume/flakes/vf/format_part.hpp>
#include <sge/renderer/vf/view.hpp>

namespace flakelib
{
namespace volume
{
namespace flakes
{
namespace vf
{
typedef
sge::renderer::vf::view<vf::format_part>
format_part_view;
}
}
}
}

#endif
