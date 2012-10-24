#include <flakelib/splatter/rectangle/object.hpp>

flakelib::splatter::rectangle::object::object(
	rectangle::position const &_position,
	rectangle::size const &_size)
:
	position_(
		_position),
	size_(
		_size)
{
}

flakelib::splatter::rectangle::position const &
flakelib::splatter::rectangle::object::position() const
{
	return position_;
}

flakelib::splatter::rectangle::size const &
flakelib::splatter::rectangle::object::size() const
{
	return size_;
}

flakelib::splatter::rectangle::object::~object()
{
}
