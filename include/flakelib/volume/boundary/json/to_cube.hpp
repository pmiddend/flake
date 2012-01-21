#ifndef FLAKELIB_VOLUME_BOUNDARY_JSON_TO_CUBE_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_JSON_TO_CUBE_HPP_INCLUDED

#include <flakelib/volume/boundary/cube/object.hpp>
#include <sge/parse/json/object_fwd.hpp>

namespace flakelib
{
namespace volume
{
namespace boundary
{
namespace json
{
boundary::cube::object const
to_cube(
	sge::parse::json::object const &);
}
}
}
}

#endif
