#include <flake/test/information/manager.hpp>
#include <sge/font/rect.hpp>
#include <sge/font/vector.hpp>
#include <sge/font/draw/static_text.hpp>
#include <boost/next_prior.hpp>
#include <sge/font/lit.hpp>
#include <sge/font/from_fcppt_string.hpp>
#include <flake/test/information/object.hpp>
#include <sge/font/parameters.hpp>
#include <sge/font/object.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text_parameters.hpp>
#include <sge/font/draw/simple.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <fcppt/text.hpp>
#include <sge/font/weight/bold.hpp>
#include <fcppt/math/box/structure_cast.hpp>

flake::test::information::manager::manager(
	sge::font::system &_font_system,
	sge::font::ttf_size const _ttf_size,
	sge::renderer::device::ffp &_renderer,
	sge::image::color::any::object const &_color)
:
	renderer_(
		_renderer),
	color_(
		_color),
	font_(
		_font_system.create_font(
			sge::font::parameters()
				.family(
					FCPPT_TEXT("sans"))
				.ttf_size(
					_ttf_size)
				.weight(
					sge::font::weight::bold()))),
	children_()
{
}

void
flake::test::information::manager::render(
	sge::renderer::context::ffp &_context)
{
	sge::font::string result;

	for(
		child_sequence::const_iterator it =
			children_.begin();
		it != children_.end();
		++it)
	{
		result +=
			sge::font::from_fcppt_string(
				it->identifier().get()+
				FCPPT_TEXT(": ")+
				it->value());

		if(it != boost::prior(children_.end()))
			result +=
				SGE_FONT_LIT("\n");
	}

	sge::font::unit const padding =
		2;

	sge::font::rect const viewport_rect(
		fcppt::math::box::structure_cast<sge::font::rect>(
			renderer_.onscreen_target().viewport().get()));

	sge::font::draw::static_text text(
		renderer_,
		*font_,
		result,
		sge::font::text_parameters(
			sge::font::align_h::left),
		sge::font::vector::null(),
		color_);

	text.pos(
		sge::font::vector(
			padding,
			viewport_rect.bottom() - text.rect().bottom() - padding));

	text.draw(
		_context);
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
