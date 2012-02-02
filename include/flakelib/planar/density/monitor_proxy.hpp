#ifndef FLAKELIB_PLANAR_DENSITY_MONITOR_PROXY_HPP_INCLUDED
#define FLAKELIB_PLANAR_DENSITY_MONITOR_PROXY_HPP_INCLUDED

#include <flakelib/buffer/planar_view_fwd.hpp>
#include <flakelib/planar/density/cursor_rectangle.hpp>
#include <flakelib/planar/monitor/dim.hpp>
#include <flakelib/planar/monitor/grid_dimensions.hpp>
#include <flakelib/planar/monitor/name.hpp>
#include <flakelib/planar/monitor/parent_fwd.hpp>
#include <flakelib/planar/monitor/planar_converter_fwd.hpp>
#include <flakelib/planar/monitor/texture.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace planar
{
namespace density
{
class monitor_proxy
{
FCPPT_NONCOPYABLE(
	monitor_proxy);
public:
	explicit
	monitor_proxy(
		monitor::parent &,
		monitor::grid_dimensions const &,
		monitor::dim const &,
		monitor::planar_converter &,
		monitor::name const &);

	void
	update(
		flakelib::buffer::planar_view<cl_float> const &);

	density::cursor_rectangle const
	rectangle() const;

	monitor::texture &
	monitor();

	~monitor_proxy();
private:
	monitor::texture density_texture_;
	monitor::planar_converter &planar_converter_;
};
}
}
}

#endif
