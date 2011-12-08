#ifndef FLAKELIB_DENSITY_MONITOR_PROXY_HPP_INCLUDED
#define FLAKELIB_DENSITY_MONITOR_PROXY_HPP_INCLUDED

#include <flakelib/density/cursor_rectangle.hpp>
#include <flakelib/monitor/dim.hpp>
#include <flakelib/monitor/grid_dimensions.hpp>
#include <flakelib/monitor/parent_fwd.hpp>
#include <flakelib/monitor/planar_converter_fwd.hpp>
#include <flakelib/monitor/texture.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
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
		monitor::planar_converter &);

	void
	update(
		sge::opencl::memory_object::image::planar &);

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

#endif
