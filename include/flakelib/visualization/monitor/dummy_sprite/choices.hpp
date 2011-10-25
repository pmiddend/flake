#ifndef FLAKELIB_VISUALIZATION_MONITOR_DUMMY_SPRITE_CHOICES_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_DUMMY_SPRITE_CHOICES_HPP_INCLUDED

#include <flakelib/visualization/monitor/scalar.hpp>
#include <sge/image/color/rgba8.hpp>
#include <sge/sprite/choices.hpp>
#include <sge/sprite/no_color.hpp>
#include <sge/sprite/type_choices.hpp>
#include <sge/sprite/with_dim.hpp>
#include <sge/sprite/with_texture.hpp>
#include <sge/sprite/intrusive/tag.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace visualization
{
namespace monitor
{
namespace dummy_sprite
{
typedef
sge::sprite::choices
<
	sge::sprite::type_choices
	<
		monitor::scalar,
		monitor::scalar,
		sge::sprite::no_color
	>,
	boost::mpl::vector3
	<
		sge::sprite::with_dim,
		sge::sprite::with_texture,
		sge::sprite::intrusive::tag
	>
>
choices;
}
}
}
}

#endif
