#ifndef FLAKELIB_VISUALIZATION_MONITOR_PLANAR_ARROWS_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_PLANAR_ARROWS_HPP_INCLUDED

#include <flakelib/buffer_or_image.hpp>
#include <flakelib/visualization/monitor/parent_fwd.hpp>
#include <flakelib/visualization/monitor/rect.hpp>
#include <flakelib/visualization/monitor/grid_scale.hpp>
#include <flakelib/visualization/monitor/position.hpp>
#include <flakelib/visualization/monitor/arrow_scale.hpp>
#include <flakelib/visualization/monitor/grid_dimensions.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/memory_object/vertex_buffer.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
class planar_arrows
{
FCPPT_NONCOPYABLE(
	planar_arrows);
public:
	explicit
	planar_arrows(
		monitor::parent &,
		monitor::grid_dimensions const &,
		monitor::arrow_scale const &,
		monitor::grid_scale const &,
		monitor::position const &);
	
	void
	from_buffer(
		flakelib::buffer_or_image const &);

	void
	render();

	monitor::rect const
	area() const;

	~planar_arrows();
private:
	monitor::parent &parent_;
	monitor::grid_dimensions const dimensions_;
	monitor::arrow_scale const arrow_scale_;
	monitor::grid_scale const grid_scale_;
	monitor::position const position_;
	sge::renderer::vertex_buffer_ptr const vb_;
	sge::opencl::memory_object::vertex_buffer cl_vb_;
};
}
}
}

#endif
