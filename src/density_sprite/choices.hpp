#ifndef FLAKE_DENSITY_SPRITE_CHOICES_HPP_INCLUDED
#define FLAKE_DENSITY_SPRITE_CHOICES_HPP_INCLUDED

#include "type_choices.hpp"
#include <sge/sprite/choices.hpp>
#include <sge/sprite/with_texture.hpp>
#include <sge/sprite/with_dim.hpp>
#include <sge/sprite/intrusive/tag.hpp>
#include <boost/mpl/vector/vector10.hpp>

namespace flake
{
namespace density_sprite
{
typedef
sge::sprite::choices
<
	density_sprite::type_choices,
	boost::mpl::vector3
	<
		sge::sprite::with_texture,
		sge::sprite::with_dim,
		sge::sprite::intrusive::tag
	>
>
choices;
}
}

#endif
