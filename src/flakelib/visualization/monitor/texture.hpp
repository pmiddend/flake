#ifndef FLAKELIB_VISUALIZATION_MONITOR_TEXTURE_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_TEXTURE_HPP_INCLUDED

#include <flakelib/visualization/monitor/dummy_sprite/object.hpp>
#include <flakelib/visualization/monitor/parent_fwd.hpp>
#include <flakelib/visualization/monitor/name.hpp>
#include <flakelib/visualization/monitor/grid_dimensions.hpp>
#include <flakelib/visualization/monitor/rect.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <flakelib/planar_object.hpp>
#include <sge/sprite/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
class texture
{
FCPPT_NONCOPYABLE(
	texture);
public:
	explicit
	texture(
		monitor::parent &,
		monitor::name const &,
		monitor::grid_dimensions const &,
		monitor::rect const &,
		sge::renderer::device &,
		sge::opencl::context::object &);

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
	
	~texture();
private:
	monitor::parent &parent_;
	fcppt::string const name_;
	sge::renderer::texture::planar_ptr renderer_texture_;
	sge::opencl::memory_object::image::planar cl_texture_;
	dummy_sprite::object sprite_;
};
}
}
}

#endif
