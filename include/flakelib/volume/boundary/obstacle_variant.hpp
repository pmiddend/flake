#ifndef FLAKELIB_VOLUME_BOUNDARY_OBSTACLE_VARIANT_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_OBSTACLE_VARIANT_HPP_INCLUDED

#include <flakelib/volume/boundary/cube/object.hpp>
#include <flakelib/volume/boundary/sphere/object.hpp>
#include <fcppt/variant/object_decl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace volume
{
namespace boundary
{
typedef
fcppt::variant::object
<
	boost::mpl::vector2
	<
		sphere::object,
		cube::object
	>
>
obstacle_variant;
}
}
}

#endif
