#ifndef FLAKELIB_VOLUME_BOUNDARY_SPHERE_OBJECT_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_SPHERE_OBJECT_HPP_INCLUDED

#include <flakelib/volume/boundary/sphere/radius.hpp>
#include <flakelib/volume/boundary/sphere/position.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

namespace flakelib
{
namespace volume
{
namespace boundary
{
namespace sphere
{
class object
{
public:
	explicit
	object(
		sphere::radius const &,
		sphere::position const &);

	sphere::radius const &
	radius() const;

	sphere::position const &
	position() const;
private:
	sphere::radius radius_;
	sphere::position position_;
};
}
}
}
}

#endif
