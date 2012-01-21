#ifndef FLAKELIB_VOLUME_BOUNDARY_JSON_TO_OBSTACLE_SEQUENCE_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_JSON_TO_OBSTACLE_SEQUENCE_HPP_INCLUDED

#include <flakelib/volume/boundary/obstacle_sequence.hpp>
#include <sge/parse/json/array_fwd.hpp>

namespace flakelib
{
namespace volume
{
namespace boundary
{
namespace json
{
boundary::obstacle_sequence const
to_obstacle_sequence(
	sge::parse::json::array const &);
}
}
}
}

#endif
