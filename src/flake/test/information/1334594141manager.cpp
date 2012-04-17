#include <flake/test/information/manager.hpp>

flake::test::information::manager::manager(
	sge::font::metrics &_font_metrics,
	sge::renderer::device &_renderer,
	sge::image::color::any::object const &_color)
{
}

void
flake::test::information::manager::render();

flake::test::information::manager::~manager();
private:
friend class information::object;

typedef
boost::intrusive::list
<
	information::object,
	boost::intrusive::constant_time_size<false>
>
child_sequence;

sge::font::metrics &font_metrics_;
child_sequence children_;

void
add_child(
	information::object &);
