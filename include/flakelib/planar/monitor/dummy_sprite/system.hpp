#ifndef FLAKELIB_PLANAR_MONITOR_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED
#define FLAKELIB_PLANAR_MONITOR_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED

#include <flakelib/planar/monitor/dummy_sprite/choices.hpp>
#include <sge/sprite/system.hpp>

namespace flakelib
{
namespace planar
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
