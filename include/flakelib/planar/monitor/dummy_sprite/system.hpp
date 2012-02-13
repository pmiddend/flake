#ifndef FLAKELIB_PLANAR_MONITOR_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED
#define FLAKELIB_PLANAR_MONITOR_DUMMY_SPRITE_SYSTEM_HPP_INCLUDED

#include <flakelib/planar/monitor/dummy_sprite/choices.hpp>
#include <sge/sprite/buffers/with_declaration_fwd.hpp>
#include <sge/sprite/buffers/single_fwd.hpp>

namespace flakelib
{
namespace planar
{
namespace monitor
{
namespace dummy_sprite
{
typedef
sge::sprite::buffers::with_declaration
<
	sge::sprite::buffers::single
	<
		dummy_sprite::choices
	>
>
system;
}
}
}
}

#endif
