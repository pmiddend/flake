#ifndef FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_OBJECT_HPP_INCLUDED
#define FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_OBJECT_HPP_INCLUDED

#include <flake/shader/pixel_profile.hpp>
#include <flake/shader/vertex_profile.hpp>
#include <flake/volume/density_visualization/raycaster/debug_output.hpp>
#include <flake/volume/density_visualization/raycaster/step_size.hpp>
#include <flakelib/volume/float_view.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/conversion/constant_addition.hpp>
#include <flakelib/volume/conversion/object_fwd.hpp>
#include <flakelib/volume/conversion/scaling_factor.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/cg/context/object_fwd.hpp>
#include <sge/cg/parameter/named.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <sge/cg/profile/object_fwd.hpp>
#include <sge/cg/program/object.hpp>
#include <sge/renderer/cg/loaded_texture_scoped_ptr.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/cg/loaded_program_scoped_ptr.hpp>
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
		sge::cg::context::object &,
		flake::shader::vertex_profile const &,
		flake::shader::pixel_profile const &,
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

	sge::cg::program::object vertex_program_;
	sge::cg::program::object pixel_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_vertex_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_pixel_program_;
	sge::cg::parameter::named mvp_parameter_;
	sge::cg::parameter::named step_size_parameter_;
	sge::cg::parameter::named elements_per_row_parameter_;
	sge::cg::parameter::named camera_is_inside_cube_parameter_;
	sge::cg::parameter::named slice_width_vertex_parameter_;
	sge::cg::parameter::named slice_width_pixel_parameter_;
	sge::cg::parameter::named texture_size_parameter_;
	sge::cg::parameter::named camera_position_parameter_;
	sge::renderer::cg::loaded_texture_scoped_ptr loaded_texture_;

	bool
	camera_is_inside_cube() const;
};
}
}
}
}

#endif

