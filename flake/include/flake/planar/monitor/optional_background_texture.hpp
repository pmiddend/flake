#ifndef FLAKE_PLANAR_MONITOR_OPTIONAL_BACKGROUND_TEXTURE_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_OPTIONAL_BACKGROUND_TEXTURE_HPP_INCLUDED

#include <sge/renderer/texture/planar_fwd.hpp>
#include <fcppt/optional/reference.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
typedef
fcppt::optional::reference<sge::renderer::texture::planar>
optional_background_texture;
}
}
}

#endif

