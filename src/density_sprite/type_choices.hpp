#ifndef FLAKE_DENSITY_SPRITE_TYPE_CHOICES_HPP_INCLUDED
#define FLAKE_DENSITY_SPRITE_TYPE_CHOICES_HPP_INCLUDED

#include "../scalar.hpp"
#include <sge/sprite/type_choices.hpp>
#include <sge/sprite/no_color.hpp>

namespace flake
{
namespace density_sprite
{
typedef
sge::sprite::type_choices
<
	flake::scalar,
	flake::scalar,
	sge::sprite::no_color
>
type_choices;
}
}

#endif
