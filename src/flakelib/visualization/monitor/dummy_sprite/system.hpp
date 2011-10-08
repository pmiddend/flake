#ifndef FLAKELIB_VISUALIZATION_MONITOR_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED

#include <flakelib/visualization/monitor/dummy_sprite/choices.hpp>
#include <sge/sprite/system.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
namespace dummy_sprite
{
typedef 
sge::sprite::system<dummy_sprite::choices>::type 
system;
}
}
}
}

#endif
