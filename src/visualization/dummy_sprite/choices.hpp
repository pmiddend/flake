#ifndef FLAKE_VISUALIZATION_DUMMY_SPRITE_CHOICES_HPP_INCLUDED
#define FLAKE_VISUALIZATION_DUMMY_SPRITE_CHOICES_HPP_INCLUDED

#include <sge/sprite/choices.hpp>
#include <sge/sprite/type_choices.hpp>
#include <sge/image/color/rgba8.hpp>
#include <sge/sprite/with_dim.hpp>
#include <sge/sprite/with_texture.hpp>
#include <sge/sprite/no_color.hpp>
#include <sge/renderer/scalar.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>

namespace flake
{
namespace visualization
{
namespace dummy_sprite
{
typedef 
sge::sprite::choices
<
	sge::sprite::type_choices
	<
		sge::renderer::scalar,
		sge::renderer::scalar,
		sge::sprite::no_color
	>,
	boost::mpl::vector2
	<
		sge::sprite::with_dim,
		sge::sprite::with_texture
	>
> 
choices;
}
}
}

#endif
