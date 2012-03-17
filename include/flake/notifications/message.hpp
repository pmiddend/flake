#ifndef FLAKE_NOTIFICATIONS_MESSAGE_HPP_INCLUDED
#define FLAKE_NOTIFICATIONS_MESSAGE_HPP_INCLUDED

#include <flake/notifications/text.hpp>
#include <flake/notifications/time_to_live.hpp>
#include <flakelib/duration.hpp>
#include <fcppt/chrono/duration_impl.hpp>

namespace flake
{
namespace notifications
{
class message
{
public:
	explicit
	message(
		notifications::text const &,
		notifications::time_to_live const &);

	notifications::text const &
	text() const;

	notifications::time_to_live const &
	time_to_live() const;

	bool
	dead() const;

	void
	update(
		flakelib::duration const &);
private:
	notifications::text text_;
	notifications::time_to_live time_to_live_;
};
}
}

#endif

