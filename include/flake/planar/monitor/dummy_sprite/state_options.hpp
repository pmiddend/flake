#ifndef FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_STATE_OPTIONS_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_DUMMY_SPRITE_STATE_OPTIONS_HPP_INCLUDED

#include <sge/sprite/state/options.hpp>
#include <flake/planar/monitor/dummy_sprite/choices.hpp>

namespace flake
{
namespace planar
{
namespace monitor
{
namespace dummy_sprite
{
typedef
sge::sprite::state::options<flake::planar::monitor::dummy_sprite::state_choices>
state_options;
}
}
}
}

#endif
