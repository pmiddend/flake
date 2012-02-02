#ifndef FLAKELIB_VOLUME_VISUALIZATION_GROUND_HPP_INCLUDED
#define FLAKELIB_VOLUME_VISUALIZATION_GROUND_HPP_INCLUDED

#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/visualization/sun_direction.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/shader/object.hpp>


namespace flakelib
{
namespace volume
{
namespace visualization
{
class ground
{
FCPPT_NONCOPYABLE(
	ground);
public:
	explicit
	ground(
		sge::renderer::device &,
		sge::camera::base &,
		sge::renderer::vertex_declaration const &,
		sge::image2d::system &,
		volume::grid_size const &,
		visualization::sun_direction const &);

	void
	render(
		sge::renderer::texture::planar_ptr);

	~ground();
private:
	sge::renderer::device &renderer_;
	sge::camera::base &camera_;
	sge::renderer::vertex_declaration const &vertex_declaration_;
	sge::shader::object shader_;
	sge::renderer::vertex_buffer_ptr const vertex_buffer_;
};
}
}
}

#endif
