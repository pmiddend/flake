#ifndef FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_FORMAT_PART_VIEW_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_MODEL_VF_FORMAT_PART_VIEW_HPP_INCLUDED

#include <flakelib/volume/visualization/model_vf/format_part.hpp>
#include <sge/renderer/vf/view.hpp>

namespace flakelib
{
namespace volume
{
namespace visualization
{
namespace model_vf
{
typedef
sge::renderer::vf::view<model_vf::format_part>
format_part_view;
}
}
}
}

#endif
