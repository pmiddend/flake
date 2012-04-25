#ifndef FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_OBJECT_HPP_INCLUDED
#define FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_OBJECT_HPP_INCLUDED

#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/conversion/constant_addition.hpp>
#include <flakelib/volume/conversion/object_fwd.hpp>
#include <flake/volume/density_visualization/raycaster/debug_output.hpp>
#include <flakelib/volume/conversion/scaling_factor.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/noncopyable.hpp>


namespace flake
{
namespace volume
{
namespace density_visualization
{
namespace raycaster
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	object(
		sge::renderer::device &,
		sge::opencl::context::object &,
		sge::camera::base const &,
		sge::image2d::system &,
		flakelib::volume::conversion::object &,
		flakelib::volume::conversion::scaling_factor const &,
		flakelib::volume::conversion::constant_addition const &,
		flakelib::volume::grid_size const &,
		flake::volume::density_visualization::raycaster::debug_output const &);

	void
	update(
		flakelib::volume::float_view const &);

	void
	render();

	~object();
private:
	sge::renderer::device &renderer_;
	sge::camera::base const &camera_;
	sge::image2d::system &image_system_;
	flakelib::volume::conversion::object &conversion_;
	flakelib::volume::grid_size grid_size_;
	flake::volume::density_visualization::raycaster::debug_output const debug_output_;
	flakelib::volume::conversion::scaling_factor const scaling_factor_;
	flakelib::volume::conversion::constant_addition const constant_addition_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	sge::renderer::vertex_buffer_scoped_ptr vertex_buffer_;
	sge::renderer::texture::planar_shared_ptr texture_;
	sge::opencl::memory_object::image::planar cl_texture_;
	sge::shader::object shader_;

	bool
	camera_is_inside_cube() const;
};
}
}
}
}

#endif

