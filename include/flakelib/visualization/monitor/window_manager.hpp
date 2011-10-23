#ifndef FLAKELIB_VISUALIZATION_MONITOR_WINDOW_MANAGER_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_WINDOW_MANAGER_HPP_INCLUDED

#include <flakelib/visualization/monitor/border_size.hpp>
#include <flakelib/visualization/monitor/child_list.hpp>
#include <flakelib/visualization/monitor/name.hpp>
#include <flakelib/visualization/monitor/rect.hpp>
#include <sge/font/metrics_fwd.hpp>
#include <sge/font/text/drawer_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/box/basic_impl.hpp>


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
		monitor::rect const &,
		monitor::border_size const &,
		sge::font::metrics &,
		sge::font::text::drawer &);

	void
	update();

	void
	render();

	void
	area(
		monitor::rect const &);

	~window_manager();
private:
	monitor::child_list &children_;
	fcppt::string const master_pane_;
	monitor::rect area_;
	monitor::border_size::value_type const border_size_;
	sge::font::metrics &metrics_;
	sge::font::text::drawer &drawer_;

	monitor::child const &
	update_master_pane();
};
}
}
}

#endif
