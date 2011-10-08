#ifndef FLAKELIB_VISUALIZATION_MONITOR_DUMMY_SPRITE_OBJECT_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_DUMMY_SPRITE_OBJECT_HPP_INCLUDED

#include <flakelib/visualization/monitor/dummy_sprite/choices.hpp>
#include <sge/sprite/object_fwd.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
namespace dummy_sprite
{
typedef
sge::sprite::object<dummy_sprite::choices>
object;
}
}
}
}

#endif
