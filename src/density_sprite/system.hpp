#ifndef FLAKE_DENSITY_SPRITE_SYSTEM_HPP_INCLUDED
#define FLAKE_DENSITY_SPRITE_SYSTEM_HPP_INCLUDED

#include "choices.hpp"
#include <sge/sprite/system.hpp>

namespace flake
{
namespace density_sprite
{
typedef
sge::sprite::system<density_sprite::choices>::type
system;
}
}

#endif
