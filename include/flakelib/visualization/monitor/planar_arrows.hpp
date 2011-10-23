#ifndef FLAKELIB_VISUALIZATION_MONITOR_PLANAR_ARROWS_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_PLANAR_ARROWS_HPP_INCLUDED

#include <flakelib/planar_object.hpp>
#include <flakelib/visualization/monitor/arrow_scale.hpp>
#include <flakelib/visualization/monitor/child.hpp>
#include <flakelib/visualization/monitor/grid_dimensions.hpp>
#include <flakelib/visualization/monitor/grid_scale.hpp>
#include <flakelib/visualization/monitor/name.hpp>
#include <flakelib/visualization/monitor/parent_fwd.hpp>
#include <flakelib/visualization/monitor/rect.hpp>
#include <flakelib/visualization/monitor/dummy_sprite/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>


namespace flakelib
{
namespace visualization
{
namespace monitor
{
class planar_arrows
:
	public monitor::child
{
FCPPT_NONCOPYABLE(
	planar_arrows);
public:
	explicit
	planar_arrows(
		monitor::parent &,
		monitor::name const &,
		monitor::grid_dimensions const &,
		monitor::arrow_scale const &,
		monitor::grid_scale const &,
		sge::renderer::texture::planar_ptr);

	void
	from_planar_object(
		flakelib::planar_object const &);

	void
	position(
		monitor::rect::vector const &);

	monitor::rect const
	area() const;

	fcppt::string const
	name() const;

	void
	render();


	~planar_arrows();
private:
	fcppt::string const name_;
	monitor::grid_dimensions::value_type const dimensions_;
	monitor::arrow_scale const arrow_scale_;
	monitor::grid_scale const grid_scale_;
	monitor::rect::vector position_;
	sge::renderer::vertex_buffer_ptr const vb_;
	sge::opencl::memory_object::buffer cl_vb_;
	fcppt::scoped_ptr<dummy_sprite::object> sprite_;
};
}
}
}

#endif
