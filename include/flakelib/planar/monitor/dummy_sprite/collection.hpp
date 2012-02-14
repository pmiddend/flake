#ifndef FLAKELIB_PLANAR_MONITOR_DUMMY_SPRITE_COLLECTION_HPP_INCLUDED
#define FLAKELIB_PLANAR_MONITOR_DUMMY_SPRITE_COLLECTION_HPP_INCLUDED

#include <flakelib/planar/monitor/dummy_sprite/choices.hpp>
#include <sge/sprite/intrusive/ordered/collection.hpp>

namespace flakelib
{
namespace planar
{
namespace monitor
{
namespace dummy_sprite
{
typedef
sge::sprite::intrusive::ordered::collection<dummy_sprite::choices,int>
collection;
}
}
}
}

#endif
