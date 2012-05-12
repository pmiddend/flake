#ifndef FLAKE_VOLUME_FLAKES_MANAGER_HPP_INCLUDED
#define FLAKE_VOLUME_FLAKES_MANAGER_HPP_INCLUDED

#include <flake/shader/pixel_profile.hpp>
#include <flake/shader/vertex_profile.hpp>
#include <flake/volume/flakes/count.hpp>
#include <flake/volume/flakes/maximum_size.hpp>
#include <flake/volume/flakes/minimum_size.hpp>
#include <flake/volume/flakes/point_size_view.hpp>
#include <flake/volume/flakes/position_view.hpp>
#include <flake/volume/flakes/texture.hpp>
#include <flake/volume/flakes/texture_tile_count.hpp>
#include <flake/volume/flakes/texture_tile_size.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/cg/context/object_fwd.hpp>
#include <sge/cg/parameter/named.hpp>
#include <sge/cg/profile/object_fwd.hpp>
#include <sge/cg/program/object.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <sge/renderer/cg/loaded_program_scoped_ptr.hpp>
#include <sge/renderer/cg/loaded_texture_scoped_ptr.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <sge/renderer/texture/planar_scoped_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>


namespace flake
{
namespace volume
{
namespace flakes
{
class manager
{
FCPPT_NONCOPYABLE(
	manager);
public:
	manager(
		sge::renderer::device &,
		sge::cg::context::object &,
		flake::shader::vertex_profile const &,
		flake::shader::pixel_profile const &,
		sge::camera::base &,
		sge::opencl::context::object &,
		sge::image2d::system &,
		flakes::count const &,
		flakes::minimum_size const &,
		flakes::maximum_size const &,
		flakes::texture const &,
		flakes::texture_tile_size const &,
		flakes::texture_tile_count const &,
		flakelib::volume::grid_size const &);

	void
	render(
		sge::renderer::context::object &);

	flakes::position_view const
	cl_positions();

	flakes::point_size_view const
	cl_point_sizes();

	flakes::minimum_size const &
	minimum_size() const;

	flakes::maximum_size const &
	maximum_size() const;

	~manager();
private:
	sge::renderer::device &renderer_;
	sge::camera::base &camera_;
	flakes::minimum_size const minimum_size_;
	flakes::maximum_size const maximum_size_;
	sge::renderer::vertex_declaration_scoped_ptr vertex_declaration_;
	sge::renderer::vertex_buffer_scoped_ptr positions_buffer_;
	sge::renderer::vertex_buffer_scoped_ptr texcoords_buffer_;
	sge::renderer::vertex_buffer_scoped_ptr point_sizes_buffer_;
	flakes::texture_tile_size tile_size_;

	sge::cg::program::object vertex_program_;
	sge::cg::program::object pixel_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_vertex_program_;
	sge::renderer::cg::loaded_program_scoped_ptr loaded_pixel_program_;
	sge::cg::parameter::named camera_position_parameter_;
	sge::cg::parameter::named tile_size_parameter_;
	sge::cg::parameter::named maximum_distance_parameter_;
	sge::cg::parameter::named mvp_parameter_;

	sge::renderer::texture::planar_scoped_ptr texture_;
	sge::renderer::cg::loaded_texture_scoped_ptr loaded_texture_;

	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> cl_positions_buffer_;
	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> cl_point_sizes_buffer_;

	void
	generate_particles(
		flakelib::volume::grid_size const &,
		flakes::texture_tile_count const &);
};
}
}
}

#endif

