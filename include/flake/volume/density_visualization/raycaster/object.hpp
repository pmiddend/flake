#ifndef FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_OBJECT_HPP_INCLUDED
#define FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_OBJECT_HPP_INCLUDED

#include <flake/shader/pair.hpp>
#include <flake/shader/parameter/matrix.hpp>
#include <flake/shader/parameter/planar_texture.hpp>
#include <flake/shader/parameter/scalar.hpp>
#include <flake/shader/parameter/vector.hpp>
#include <flake/volume/density_visualization/raycaster/debug_output.hpp>
#include <flake/volume/density_visualization/raycaster/step_size.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/conversion/constant_addition.hpp>
#include <flakelib/volume/conversion/object_fwd.hpp>
#include <flakelib/volume/conversion/scaling_factor.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/cg/loaded_program_scoped_ptr.hpp>
#include <sge/renderer/cg/loaded_texture_scoped_ptr.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
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
		flake::shader::context &,
		sge::opencl::context::object &,
		sge::camera::base const &,
		sge::image2d::system &,
		flakelib::volume::conversion::object &,
		flakelib::volume::grid_size const &,
		flake::volume::density_visualization::raycaster::step_size const &,
		flake::volume::density_visualization::raycaster::debug_output const &);

	void
	update(
		flakelib::volume::float_view const &,
		flakelib::volume::conversion::scaling_factor const &,
		flakelib::volume::conversion::constant_addition const &);

	void
	render(
		sge::renderer::context::object &);

	~object();
private:
	sge::renderer::device &renderer_;
	sge::camera::base const &camera_;
	sge::image2d::system &image_system_;
	flakelib::volume::conversion::object &conversion_;
	flakelib::volume::grid_size grid_size_;
	flake::volume::density_visualization::raycaster::debug_output const debug_output_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	sge::renderer::vertex_buffer_scoped_ptr vertex_buffer_;
	sge::renderer::texture::planar_shared_ptr texture_;
	sge::opencl::memory_object::image::planar cl_texture_;

	flake::shader::pair shader_;
	flake::shader::parameter::matrix<sge::renderer::scalar,4u,4u> mvp_parameter_;
	flake::shader::parameter::scalar<sge::renderer::scalar> step_size_parameter_;
	flake::shader::parameter::scalar<int> elements_per_row_parameter_;
	flake::shader::parameter::scalar<sge::renderer::scalar> camera_is_inside_cube_parameter_;
	flake::shader::parameter::scalar<sge::renderer::scalar> slice_width_vertex_parameter_;
	flake::shader::parameter::scalar<int> slice_width_pixel_parameter_;
	flake::shader::parameter::vector<sge::renderer::scalar,2> texture_size_parameter_;
	flake::shader::parameter::vector<sge::renderer::scalar,3> camera_position_parameter_;
	flake::shader::parameter::planar_texture loaded_texture_;

	bool
	camera_is_inside_cube() const;
};
}
}
}
}

#endif

