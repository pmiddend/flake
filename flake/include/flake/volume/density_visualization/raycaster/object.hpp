#ifndef FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_OBJECT_HPP_INCLUDED
#define FLAKE_VOLUME_DENSITY_VISUALIZATION_RAYCASTER_OBJECT_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
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
#include <sge/renderer/cg/loaded_program_unique_ptr.hpp>
#include <sge/renderer/cg/loaded_texture_unique_ptr.hpp>
#include <sge/renderer/context/core_fwd.hpp>
#include <sge/renderer/device/core_fwd.hpp>
#include <sge/renderer/state/core/blend/object_unique_ptr.hpp>
#include <sge/renderer/state/core/sampler/object_unique_ptr.hpp>
#include <sge/renderer/texture/planar_shared_ptr.hpp>
#include <sge/renderer/vertex/buffer_unique_ptr.hpp>
#include <sge/renderer/vertex/declaration_unique_ptr.hpp>
#include <sge/shader/pair.hpp>
#include <sge/shader/parameter/matrix.hpp>
#include <sge/shader/parameter/planar_texture.hpp>
#include <sge/shader/parameter/scalar.hpp>
#include <sge/shader/parameter/vector.hpp>
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
	FLAKE_DETAIL_SYMBOL
	object(
		sge::renderer::device::core &,
		sge::shader::context &,
		sge::opencl::context::object &,
		sge::camera::base const &,
		sge::image2d::system &,
		flakelib::volume::conversion::object &,
		flakelib::volume::grid_size const &,
		flake::volume::density_visualization::raycaster::step_size const &,
		flake::volume::density_visualization::raycaster::debug_output const &);

	FLAKE_DETAIL_SYMBOL
	void
	update(
		flakelib::volume::float_view const &,
		flakelib::volume::conversion::scaling_factor const &,
		flakelib::volume::conversion::constant_addition const &);

	FLAKE_DETAIL_SYMBOL
	void
	render(
		sge::renderer::context::core &);

	FLAKE_DETAIL_SYMBOL
	~object();
private:
	sge::renderer::device::core &renderer_;
	sge::camera::base const &camera_;
	sge::image2d::system &image_system_;
	flakelib::volume::conversion::object &conversion_;
	sge::renderer::state::core::blend::object_unique_ptr const blend_state_;
	sge::renderer::state::core::sampler::object_unique_ptr const sampler_state_;
	flakelib::volume::grid_size grid_size_;
	flake::volume::density_visualization::raycaster::debug_output const debug_output_;
	sge::renderer::vertex::declaration_unique_ptr vertex_declaration_;
	sge::renderer::vertex::buffer_unique_ptr vertex_buffer_;
	sge::renderer::texture::planar_shared_ptr texture_;
	sge::opencl::memory_object::image::planar cl_texture_;

	sge::shader::pair shader_;
	sge::shader::parameter::matrix<sge::renderer::scalar,4u,4u> mvp_parameter_;
	sge::shader::parameter::scalar<sge::renderer::scalar> step_size_parameter_;
	sge::shader::parameter::scalar<int> elements_per_row_parameter_;
	sge::shader::parameter::scalar<sge::renderer::scalar> camera_is_inside_cube_parameter_;
	sge::shader::parameter::scalar<sge::renderer::scalar> slice_width_vertex_parameter_;
	sge::shader::parameter::scalar<int> slice_width_pixel_parameter_;
	sge::shader::parameter::vector<sge::renderer::scalar,2> texture_size_parameter_;
	sge::shader::parameter::vector<sge::renderer::scalar,3> camera_position_parameter_;
	sge::shader::parameter::planar_texture loaded_texture_;

	bool
	camera_is_inside_cube() const;
};
}
}
}
}

#endif

