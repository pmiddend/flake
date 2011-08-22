#include "source.hpp"

flake::source::source(
	position_type const &_position,
	flake::scalar const _intensity)
:
	position_(
		_position),
	intensity_(
		_intensity)
{
}

flake::source::position_type const &
flake::source::position() const
{
	return position_;
}

flake::scalar
flake::source::intensity() const
{
	return intensity_;
}
