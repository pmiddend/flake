#include <flakelib/splatter/circle/object.hpp>

flakelib::splatter::circle::object::object(
	circle::position const &_position,
	circle::radius const &_radius)
:
	position_(
		_position),
	radius_(
		_radius)
{
}

flakelib::splatter::circle::position const &
flakelib::splatter::circle::object::position() const
{
	return position_;
}

flakelib::splatter::circle::radius const &
flakelib::splatter::circle::object::radius() const
{
	return radius_;
}

flakelib::splatter::circle::object::~object()
{
}
