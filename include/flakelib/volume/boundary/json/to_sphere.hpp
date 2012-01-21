#ifndef FLAKELIB_VOLUME_BOUNDARY_JSON_TO_SPHERE_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_JSON_TO_SPHERE_HPP_INCLUDED

#include <flakelib/volume/boundary/sphere/object.hpp>
#include <sge/parse/json/object_fwd.hpp>

namespace flakelib
{
namespace volume
{
namespace boundary
{
namespace json
{
boundary::sphere::object const
to_sphere(
	sge::parse::json::object const &);
}
}
}
}

#endif
