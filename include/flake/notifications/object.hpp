#ifndef FLAKE_NOTIFICATIONS_OBJECT_HPP_INCLUDED
#define FLAKE_NOTIFICATIONS_OBJECT_HPP_INCLUDED

#include <flake/notifications/message.hpp>
#include <flake/notifications/text.hpp>
#include <flake/notifications/time_to_live.hpp>
#include <sge/font/object_scoped_ptr.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/font/ttf_size.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <sge/timer/basic_impl.hpp>
#include <sge/timer/clocks/standard.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/string.hpp>
#include <fcppt/config/external_begin.hpp>
#include <list>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace notifications
{
// The notification object is currently hard-coded to use the renderer's
// onscreen target to get the viewport. Can be unhardcoded very easily.
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	// We take the font metrics as a shared pointer here because we don't
	// want the notification object to have sole ownership over the
	// metrics. However, a ref might be inconvenient, too, if we have a
	// single font just for the notifications.
	object(
		sge::renderer::device &,
		sge::font::system &,
		sge::font::ttf_size const &,
		notifications::time_to_live const &);

	void
	add_message(
		notifications::text const &);

	void
	update();

	void
	render(
		sge::renderer::context::object &);

	~object();
private:
	typedef
	std::list<notifications::message>
	message_sequence;

	notifications::time_to_live const time_to_live_;
	sge::renderer::device &renderer_;
	sge::font::object_scoped_ptr font_;
	message_sequence messages_;
	sge::timer::basic<sge::timer::clocks::standard> second_timer_;
};
}
}

#endif

