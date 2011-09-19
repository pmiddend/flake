#ifndef FLAKE_VISUALIZATION_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED
#define FLAKE_VISUALIZATION_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED

#include "choices.hpp"
#include <sge/sprite/system.hpp>

namespace flake
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
