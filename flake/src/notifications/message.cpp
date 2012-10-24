#include <flake/notifications/message.hpp>

flake::notifications::message::message(
	notifications::text const &_text,
	notifications::time_to_live const &_time_to_live)
:
	text_(
		_text),
	time_to_live_(
		_time_to_live)
{
}

flake::notifications::text const &
flake::notifications::message::text() const
{
	return text_;
}

flake::notifications::time_to_live const &
flake::notifications::message::time_to_live() const
{
	return time_to_live_;
}

bool
flake::notifications::message::dead() const
{
	return
		time_to_live_.get() <
		flakelib::duration(
			0.0f);
}

void
flake::notifications::message::update(
	flakelib::duration const &_duration)
{
	time_to_live_ -=
		notifications::time_to_live(
			_duration);
}
