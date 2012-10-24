#ifndef FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_STATE_OBJECT_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_STATE_OBJECT_HPP_INCLUDED

#include <flake/planar/monitor/dummy_sprite/state_choices.hpp>
#include <sge/sprite/state/object.hpp>

namespace flake
{
namespace planar
{
namespace monitor
{
namespace dummy_sprite
{
typedef
sge::sprite::state::object
<
	flake::planar::monitor::dummy_sprite::state_choices
>
state_object;
}
}
}
}

#endif
