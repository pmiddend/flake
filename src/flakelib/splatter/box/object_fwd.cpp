#include <flakelib/splatter/box/object.hpp>

flakelib::splatter::box::object::object(
	box::position const &_position,
	box::size const &_size)
:
	position_(
		_position),
	size_(
		_size)
{
}

flakelib::splatter::box::position const &
flakelib::splatter::box::object::position() const
{
	return position_;
}

flakelib::splatter::box::size const &
flakelib::splatter::box::object::size() const
{
	return size_;
}

flakelib::splatter::box::object::~object()
{
}
