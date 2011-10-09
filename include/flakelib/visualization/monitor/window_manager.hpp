#ifndef FLAKELIB_VISUALIZATION_MONITOR_WINDOW_MANAGER_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_WINDOW_MANAGER_HPP_INCLUDED

#include <flakelib/visualization/monitor/child_list.hpp>
#include <flakelib/visualization/monitor/rect.hpp>
#include <flakelib/visualization/monitor/name.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
class window_manager
{
FCPPT_NONCOPYABLE(
	window_manager);
public:
	explicit
	window_manager(
		monitor::child_list &,
		monitor::name const &master_pane,
		monitor::rect const &);

	void
	update();

	void
	area(
		monitor::rect const &);

	~window_manager();
private:
	monitor::child_list &children_;
	fcppt::string const master_pane_;
	monitor::rect area_;

	monitor::child const &
	update_master_pane();
};
}
}
}

#endif
