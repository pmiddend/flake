#ifndef FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_STATE_PARAMETERS_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_STATE_PARAMETERS_HPP_INCLUDED

#include <flake/planar/monitor/dummy_sprite/state_choices.hpp>
#include <sge/sprite/state/parameters.hpp>

namespace flake
{
namespace planar
{
namespace monitor
{
namespace dummy_sprite
{
typedef
sge::sprite::state::parameters
<
	flake::planar::monitor::dummy_sprite::state_choices
>
state_parameters;
}
}
}
}

#endif
