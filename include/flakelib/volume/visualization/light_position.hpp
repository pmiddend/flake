#ifndef FLAKELIB_VOLUME_VISUALIZATION_LIGHT_POSITION_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_LIGHT_POSITION_HPP_INCLUDED

#include <sge/renderer/vector3.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace volume
{
namespace visualization
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::renderer::vector3,
	light_position);
}
}
}

#endif
