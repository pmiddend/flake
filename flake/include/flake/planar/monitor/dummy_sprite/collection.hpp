#ifndef FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_COLLECTION_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_COLLECTION_HPP_INCLUDED

#include <flake/planar/monitor/dummy_sprite/choices.hpp>
#include <flake/planar/monitor/dummy_sprite/order.hpp>
#include <sge/sprite/intrusive/ordered/collection.hpp>

namespace flake
{
namespace planar
{
namespace monitor
{
namespace dummy_sprite
{
typedef
sge::sprite::intrusive::ordered::collection
<
	flake::planar::monitor::dummy_sprite::choices,
	flake::planar::monitor::dummy_sprite::order
>
collection;
}
}
}
}

#endif
