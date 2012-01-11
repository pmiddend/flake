#include <flakelib/volume/boundary/cube/object.hpp>

flakelib::volume::boundary::cube::object::object(
	cube::width const &_width,
	cube::position const &_position)
:
	width_(
		_width),
	position_(
		_position)
{
}

flakelib::volume::boundary::cube::width const &
flakelib::volume::boundary::cube::object::width() const
{
	return width_;
}

flakelib::volume::boundary::cube::position const &
flakelib::volume::boundary::cube::object::position() const
{
	return position_;
}
