#include <flakelib/visualization/monitor/window_manager.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/vector/output.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <iostream>
#include <fcppt/config/external_end.hpp>

flakelib::visualization::monitor::window_manager::window_manager(
	monitor::child_list &_children,
	monitor::name const &_master_pane,
	monitor::rect const &_rect)
:
	children_(
		_children),
	master_pane_(
		_master_pane.get()),
	area_(
		_rect)
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
		if(current_pos.x() + it->area().w() > remaining_rect.right())
		{
			FCPPT_ASSERT_PRE(
				current_pos.y() + it->area().h() <= remaining_rect.bottom());

			current_pos.x() = 
				remaining_rect.left();

			current_pos.y() += 
				current_row_max_height;

			current_row_max_height = 
				0;
		}
		 
	//	std::cout << "Pane \"" << it->name() << "\" got position: " << current_pos << "\n";

		it->position(
			current_pos);

		current_pos.x() += it->area().w();

		current_row_max_height = 
			std::max(
				current_row_max_height,
				it->area().h());
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

	//std::cout << "Master pane got position: " << master_pane->area().pos() << "\n";

	return *master_pane;
}
