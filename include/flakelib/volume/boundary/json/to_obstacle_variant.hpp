#ifndef FLAKELIB_VOLUME_BOUNDARY_JSON_TO_OBSTACLE_VARIANT_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_JSON_TO_OBSTACLE_VARIANT_HPP_INCLUDED

#include <flakelib/volume/boundary/obstacle_variant.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <fcppt/variant/object_impl.hpp>


namespace flakelib
{
namespace volume
{
namespace boundary
{
namespace json
{
boundary::obstacle_variant const
to_obstacle_variant(
	sge::parse::json::object const &);
}
}
}
}

#endif
