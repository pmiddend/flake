#include <flakelib/volume/boundary/cube/object.hpp>

flakelib::volume::boundary::cube::object::object(
	cube::size const &_size,
	cube::position const &_position)
:
	size_(
		_size),
	position_(
		_position)
{
}

flakelib::volume::boundary::cube::size const &
flakelib::volume::boundary::cube::object::size() const
{
	return size_;
}

flakelib::volume::boundary::cube::position const &
flakelib::volume::boundary::cube::object::position() const
{
	return position_;
}
