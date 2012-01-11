#include <flakelib/volume/boundary/sphere/object.hpp>

flakelib::volume::boundary::sphere::object::object(
	sphere::radius const &_radius,
	sphere::position const &_position)
:
	radius_(
		_radius),
	position_(
		_position)
{
}

flakelib::volume::boundary::sphere::radius const &
flakelib::volume::boundary::sphere::object::radius() const
{
	return
		radius_;
}

flakelib::volume::boundary::sphere::position const &
flakelib::volume::boundary::sphere::object::position() const
{
	return
		position_;
}
