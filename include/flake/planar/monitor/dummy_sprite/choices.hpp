#ifndef FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_CHOICES_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_CHOICES_HPP_INCLUDED

#include <flake/planar/monitor/scalar.hpp>
#include <sge/sprite/config/choices.hpp>
#include <sge/sprite/config/float_type.hpp>
#include <sge/sprite/config/intrusive.hpp>
#include <sge/sprite/config/normal_size.hpp>
#include <sge/sprite/config/texture_coordinates.hpp>
#include <sge/sprite/config/texture_level_count.hpp>
#include <sge/sprite/config/type_choices.hpp>
#include <sge/sprite/config/unit_type.hpp>
#include <sge/sprite/config/with_texture.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
namespace dummy_sprite
{
typedef
sge::sprite::config::choices
<
	sge::sprite::config::type_choices
	<
		sge::sprite::config::unit_type<monitor::scalar>,
		sge::sprite::config::float_type<monitor::scalar>
	>,
	sge::sprite::config::normal_size,
	boost::mpl::vector2
	<
		sge::sprite::config::with_texture
		<
			sge::sprite::config::texture_level_count<1u>,
			sge::sprite::config::texture_coordinates::automatic,
			sge::sprite::config::texture_ownership::shared
		>,
		sge::sprite::config::intrusive
	>
>
choices;
}
}
}
}

#endif
