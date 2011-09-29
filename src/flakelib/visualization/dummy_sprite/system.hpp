#ifndef FLAKELIB_VISUALIZATION_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED

#include "choices.hpp"
#include <sge/sprite/system.hpp>

namespace flakelib
{
namespace visualization
{
namespace dummy_sprite
{
typedef 
sge::sprite::system<dummy_sprite::choices>::type 
system;
}
}
}

#endif
