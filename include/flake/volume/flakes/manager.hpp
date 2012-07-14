#ifndef FLAKE_VOLUME_FLAKES_MANAGER_HPP_INCLUDED
#define FLAKE_VOLUME_FLAKES_MANAGER_HPP_INCLUDED

#include <flake/shader/context_fwd.hpp>
#include <flake/shader/pair.hpp>
#include <flake/shader/parameter/vector.hpp>
#include <flake/shader/parameter/matrix.hpp>
#include <flake/shader/parameter/scalar.hpp>
#include <flake/shader/parameter/planar_texture.hpp>
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
#include <sge/image2d/system_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
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
		flake::shader::context &,
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
	flake::shader::pair shader_;
	flake::shader::parameter::vector<sge::renderer::scalar,3u> camera_position_parameter_;
	flake::shader::parameter::scalar<sge::renderer::scalar> tile_size_parameter_;
	flake::shader::parameter::scalar<sge::renderer::scalar> maximum_distance_parameter_;
	flake::shader::parameter::matrix<sge::renderer::scalar,4u,4u> mvp_parameter_;
	sge::renderer::texture::planar_scoped_ptr texture_;
	flake::shader::parameter::planar_texture loaded_texture_;
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

