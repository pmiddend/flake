#include <flake/notifications/object.hpp>
#include <sge/font/from_fcppt_string.hpp>
#include <sge/font/object.hpp>
#include <sge/font/parameters.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text.hpp>
#include <sge/font/text_parameters.hpp>
#include <sge/font/align_h/max_width.hpp>
#include <sge/font/align_h/right.hpp>
#include <sge/font/draw/static_text.hpp>
#include <sge/image/color/predef.hpp>
#include <sge/image/color/rgba8.hpp>
#include <sge/image/color/any/object.hpp>
#include <sge/image/color/init/alpha.hpp>
#include <sge/image/color/init/blue.hpp>
#include <sge/image/color/init/green.hpp>
#include <sge/image/color/init/red.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <sge/renderer/texture/emulate_srgb.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters_impl.hpp>
#include <fcppt/math/box/object_impl.hpp>
#include <fcppt/math/box/structure_cast.hpp>
#include <fcppt/variant/object.hpp>
#include <fcppt/config/external_begin.hpp>
#include <chrono>
#include <fcppt/config/external_end.hpp>


flake::notifications::object::object(
	sge::renderer::device::ffp &_renderer,
	sge::font::system &_font_system,
	sge::font::ttf_size const &_ttf_size,
	notifications::time_to_live const &_time_to_live)
:
	time_to_live_(
		_time_to_live),
	renderer_(
		_renderer),
	font_(
		_font_system.create_font(
			sge::font::parameters()
				.family(
					FCPPT_TEXT("sans"))
				.ttf_size(
					_ttf_size))),
	messages_(),
	second_timer_(
		sge::timer::parameters<sge::timer::clocks::standard>(
			std::chrono::seconds(
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
	sge::renderer::context::ffp &_context)
{
	// When drawing a font, we have to specify a rectangle to draw into.
	// Since we're not getting such a rectangle in the constructor (which
	// is controversial anyway, since the viewport might change size and
	// position), we have to construct one ourselves.
	//
	// For that, we choose an arbitrary padding value and start at the top
	// left. The rectangle is as big as the viewport is.
	sge::font::unit const
		border_padding =
			5,
		line_spacing =
			3;

	sge::font::rect const viewport_rect(
		fcppt::math::box::structure_cast<sge::font::rect>(
			renderer_.onscreen_target().viewport().get()));

	if(!viewport_rect.content())
		return;

	sge::font::text_parameters const text_parameters(
		sge::font::text_parameters(
			sge::font::align_h::right(
				sge::font::align_h::max_width(
					viewport_rect.w() - border_padding))));

	sge::font::unit current_y =
		border_padding;

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

		sge::image::color::any::object const text_color(
			sge::image::color::rgba8(
				(sge::image::color::init::red() %= 0.0f)
				(sge::image::color::init::green() %= 0.5411f)
				(sge::image::color::init::blue() %= 1.0f)
				(sge::image::color::init::alpha() %= alpha_value)));

		sge::font::string const text_content(
			sge::font::from_fcppt_string(
				it->text().get()));

		sge::font::draw::static_text static_text(
			renderer_,
			*font_,
			text_content,
			text_parameters,
			sge::font::vector::null(),
			text_color,
			sge::renderer::texture::emulate_srgb::no);

		static_text.pos(
			sge::font::vector(
				viewport_rect.left(),
				current_y));

		static_text.draw(
			_context);

		current_y +=
			static_text.rect().h() +
			line_spacing;
	}
}

flake::notifications::object::~object()
{
}
