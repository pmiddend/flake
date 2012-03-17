#include <flake/notifications/object.hpp>
#include <flake/time_modifier/object.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_event.hpp>
#include <fcppt/insert_to_fcppt_string.hpp>
#include <fcppt/text.hpp>
#include <fcppt/tr1/functional.hpp>

flake::time_modifier::object::object(
	sge::input::keyboard::device &_keyboard,
	notifications::object &_notifications)
:
	kernel_(),
	key_connection_(
		_keyboard.key_callback(
			std::tr1::bind(
				&object::key_callback,
				this,
				std::tr1::placeholders::_1))),
	notifications_(
		_notifications)
{
}

flake::time_modifier::multiplier const
flake::time_modifier::object::current_multiplier()
{
	return kernel_.current_multiplier();
}

flake::time_modifier::object::~object()
{
}

void
flake::time_modifier::object::key_callback(
	sge::input::keyboard::key_event const &e)
{
	if(!e.pressed())
		return;

	fcppt::string const multiplier_string =
		kernel_.current_multiplier().get().denominator() == 1
		?
			fcppt::insert_to_fcppt_string(
				kernel_.current_multiplier().get().numerator())
		:
			fcppt::insert_to_fcppt_string(
				kernel_.current_multiplier().get());

	switch(e.key_code())
	{
		case sge::input::keyboard::key_code::space:
			notifications_.add_message(
				notifications::text(
					FCPPT_TEXT("toggle pause")));
			kernel_.toggle_pause();
			break;
		case sge::input::keyboard::key_code::add:
			kernel_.faster();
			notifications_.add_message(
				notifications::text(
					FCPPT_TEXT("time ")+
					multiplier_string));
			break;
		case sge::input::keyboard::key_code::minus:
			kernel_.slower();
			notifications_.add_message(
				notifications::text(
					FCPPT_TEXT("time ")+
					multiplier_string));
			break;
		default:
			break;
	}
}
