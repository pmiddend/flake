#ifndef FLAKE_PLANAR_MONITOR_OPTIONAL_BACKGROUND_TEXTURE_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_OPTIONAL_BACKGROUND_TEXTURE_HPP_INCLUDED

#include <sge/renderer/texture/planar_fwd.hpp>
#include <fcppt/optional/object_fwd.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
typedef
fcppt::optional::object<sge::renderer::texture::planar &>
optional_background_texture;
}
}
}

#endif

