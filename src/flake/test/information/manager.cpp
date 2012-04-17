#include <flake/test/information/manager.hpp>
#include <flake/test/information/object.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/from_fcppt_string.hpp>
#include <sge/font/text/part.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/onscreen_target.hpp>
#include <fcppt/text.hpp>
#include <fcppt/math/box/structure_cast.hpp>


flake::test::information::manager::manager(
	sge::font::metrics &_font_metrics,
	sge::renderer::device &_renderer,
	sge::image::color::any::object const &_color)
:
	renderer_(
		_renderer),
	font_metrics_(
		_font_metrics),
	font_drawer_(
		_renderer,
		_color)
{
}

void
flake::test::information::manager::render()
{
	sge::font::text::string result;

	for(
		child_sequence::const_iterator it =
			children_.begin();
		it != children_.end();
		++it)
		result +=
			sge::font::text::from_fcppt_string(
				it->identifier().get()+
				FCPPT_TEXT(": ")+
				it->value()+
				FCPPT_TEXT("\n"));

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

	sge::font::text::draw(
		font_metrics_,
		font_drawer_,
		result,
		current_font_rect,
		sge::font::text::align_h::left,
		sge::font::text::align_v::bottom,
		sge::font::text::flags::none);
}

flake::test::information::manager::~manager()
{
}

void
flake::test::information::manager::add_child(
	information::object &_information)
{
	children_.push_back(
		_information);
}
