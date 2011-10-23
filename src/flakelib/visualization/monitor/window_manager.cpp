#include <flakelib/visualization/monitor/window_manager.hpp>
#include <sge/font/metrics.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/from_fcppt_string.hpp>
#include <sge/font/text/part.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <fcppt/config/external_end.hpp>


flakelib::visualization::monitor::window_manager::window_manager(
	monitor::child_list &_children,
	monitor::name const &_master_pane,
	monitor::rect const &_rect,
	monitor::border_size const &_border_size,
	sge::font::metrics &_metrics,
	sge::font::text::drawer &_drawer)
:
	children_(
		_children),
	master_pane_(
		_master_pane.get()),
	area_(
		_rect),
	border_size_(
		_border_size.get()),
	metrics_(
		_metrics),
	drawer_(
		_drawer)
{
}

void
flakelib::visualization::monitor::window_manager::update()
{
	monitor::child const &master_pane =
		this->update_master_pane();

	monitor::rect const remaining_rect(
		monitor::rect::vector(
			master_pane.area().right(),
			area_.top()),
		monitor::rect::dim(
			area_.w() - master_pane.area().w(),
			area_.h()));

	monitor::rect::vector current_pos(
		remaining_rect.pos());

	monitor::rect::value_type
		current_row_max_height =
			0;

	for(
		monitor::child_list::iterator it = children_.begin();
		it != children_.end();
		++it)
	{
		if(&(*it) == &master_pane)
			continue;

		// Next line
		if(current_pos.x() + border_size_ + it->area().w() > remaining_rect.right())
		{
			FCPPT_ASSERT_PRE(
				current_pos.y() + it->area().h() <= remaining_rect.bottom());

			current_pos.x() =
				remaining_rect.left();

			current_pos.y() +=
				current_row_max_height + border_size_;

			current_row_max_height =
				0;
		}

		it->position(
			monitor::rect::vector(
				current_pos.x() + border_size_,
				current_pos.y() + static_cast<monitor::rect::value_type>(metrics_.line_height())));


		current_pos.x() += it->area().w() + border_size_;

		current_row_max_height =
			std::max(
				current_row_max_height,
				it->area().h() + static_cast<monitor::rect::value_type>(metrics_.line_height()));
	}
}

void
flakelib::visualization::monitor::window_manager::render()
{
	for(
		monitor::child_list::const_iterator it = children_.begin();
		it != children_.end();
		++it)
	{
		sge::font::text::draw(
			metrics_,
			drawer_,
			sge::font::text::from_fcppt_string(
				it->name()),
			sge::font::rect(
				sge::font::rect::vector(
					it->area().pos().x(),
					it->area().pos().y() - metrics_.line_height()),
				sge::font::rect::dim(
					1000,
					1000)),
				sge::font::text::align_h::left,
				sge::font::text::align_v::top,
				sge::font::text::flags::none);
	}
}

void
flakelib::visualization::monitor::window_manager::area(
	monitor::rect const &_area)
{
	area_ =
		_area;
}

flakelib::visualization::monitor::window_manager::~window_manager()
{
}

flakelib::visualization::monitor::child const &
flakelib::visualization::monitor::window_manager::update_master_pane()
{
	monitor::child *master_pane = 0;

	for(
		monitor::child_list::iterator it =
			children_.begin();
		it != children_.end();
		++it)
	{
		if(it->name() == master_pane_)
		{
			master_pane = &(*it);
			break;
		}
	}

	FCPPT_ASSERT_PRE(
		master_pane);

	master_pane->position(
		monitor::rect::vector(
			area_.left(),
			(area_.top() + area_.bottom())/static_cast<monitor::rect::value_type>(2) -
			master_pane->area().h()/static_cast<monitor::rect::value_type>(2)));

	return *master_pane;
}
