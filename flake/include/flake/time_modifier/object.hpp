#ifndef FLAKE_TIME_MODIFIER_OBJECT_HPP_INCLUDED
#define FLAKE_TIME_MODIFIER_OBJECT_HPP_INCLUDED

#include <flake/notifications/object_fwd.hpp>
#include <flake/time_modifier/kernel.hpp>
#include <sge/input/keyboard/device_fwd.hpp>
#include <sge/input/keyboard/key_event_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/signal/scoped_connection.hpp>


namespace flake
{
namespace time_modifier
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	object(
		sge::input::keyboard::device &,
		notifications::object &,
		bool is_paused);

	time_modifier::multiplier const
	current_multiplier();

	~object();
private:
	flake::time_modifier::kernel kernel_;
	fcppt::signal::scoped_connection key_connection_;
	notifications::object &notifications_;

	void
	key_callback(
		sge::input::keyboard::key_event const &);
};
}
}

#endif

