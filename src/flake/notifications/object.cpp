#include <sge/renderer/target/onscreen.hpp>
#include <flake/notifications/object.hpp>
#include <sge/font/metrics.hpp>
#include <sge/font/rect.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/drawer_3d.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/from_fcppt_string.hpp>
#include <sge/font/text/part.hpp>
#include <sge/image/colors.hpp>
#include <sge/image/color/init.hpp>
#include <sge/image/color/rgba8.hpp>
#include <sge/renderer/device.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters_impl.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/math/box/object_impl.hpp>
#include <fcppt/math/box/structure_cast.hpp>


flake::notifications::object::object(
	sge::renderer::device &_renderer,
	sge::font::metrics &_metrics,
	notifications::time_to_live const &_time_to_live)
:
	time_to_live_(
		_time_to_live),
	renderer_(
		_renderer),
	font_metrics_(
		_metrics),
	font_drawer_(
		fcppt::make_unique_ptr<sge::font::text::drawer_3d>(
			fcppt::ref(
				renderer_),
			// The color is deliberately hard-coded here instead of being a ctor
			// parameter. This way, I can decide to make the color
			// a property of the message, not the notification manager.
			sge::image::colors::white())),
	messages_(),
	second_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			boost::chrono::seconds(
				1)))
{
}

void
flake::notifications::object::add_message(
	notifications::text const &_text)
{
	messages_.push_back(
		notifications::message(
			_text,
			time_to_live_));
}

void
flake::notifications::object::update()
{
	flakelib::duration const delta(
		sge::timer::elapsed_and_reset<flakelib::duration>(
			second_timer_));

	for(
		message_sequence::iterator it =
			messages_.begin();
		it != messages_.end();)
	{
		it->update(
			delta);

		if(it->dead())
			it =
				messages_.erase(
					it);
		else
			++it;
	}
}

void
flake::notifications::object::render(
	sge::renderer::context::object &_context)
{
	// When drawing a font, we have to specify a rectangle to draw into.
	// Since we're not getting such a rectangle in the constructor (which
	// is controversial anyway, since the viewport might change size and
	// position), we have to construct one ourselves.
	//
	// For that, we choose an arbitrary padding value and start at the top
	// left. The rectangle is as big as the viewport is.
	sge::font::unit const padding =
		5;

	sge::font::rect const viewport_rect(
		fcppt::math::box::structure_cast<sge::font::rect>(
			renderer_.onscreen_target().viewport().get()));

	sge::font::rect current_font_rect(
		sge::font::pos(
			padding,
			padding),
		viewport_rect.size());


	for(
		message_sequence::const_iterator it =
			messages_.begin();
		it != messages_.end();
		++it)
	{
		// This is a bit tricky: duration<T,U> / duration<T,U> returns
		// T (in this case T = float). However, since we're dealing
		// with strong typedefs to durations here, we have to unpack
		// them first and then do the calculations.
		flakelib::duration::rep const alpha_value =
			(it->time_to_live().get() / time_to_live_.get());

		sge::image::color::any::object const font_color(
			sge::image::color::rgba8(
				(sge::image::color::init::red() %= 0.0f)
				(sge::image::color::init::green() %= 0.5411f)
				(sge::image::color::init::blue() %= 1.0f)
				(sge::image::color::init::alpha() %= alpha_value)));

		font_drawer_->color(
			font_color);

		sge::font::text::draw(
			_context,
			font_metrics_,
			*font_drawer_,
			sge::font::text::from_fcppt_string(
				it->text().get()),
			current_font_rect,
			sge::font::text::align_h::right,
			sge::font::text::align_v::top,
			sge::font::text::flags::none);

		current_font_rect.top(
			current_font_rect.top() +
			font_metrics_.line_height());
	}
}

flake::notifications::object::~object()
{
}
