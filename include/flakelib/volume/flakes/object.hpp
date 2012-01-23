#ifndef FLAKELIB_VOLUME_FLAKES_OBJECT_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_OBJECT_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/buffer/volume_view_fwd.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/boundary/view.hpp>
#include <flakelib/volume/flakes/particle_count.hpp>
#include <flakelib/volume/flakes/particle_maximum_size.hpp>
#include <flakelib/volume/flakes/particle_minimum_size.hpp>
#include <flakelib/volume/flakes/snow_texture_size.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/camera/base_fwd.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/container/array.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_array.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace volume
{
namespace flakes
{
/**
\warning
Currently, this class assumes that the cube is at the origin and has 1.0 grid
scale (so that float 3D coordinates are convertible to 3D indexes
*/
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	explicit
	object(
		sge::renderer::device &,
		sge::camera::base &,
		sge::image2d::system &,
		sge::opencl::command_queue::object &,
		boundary::view const &,
		flakelib::build_options const &,
		flakes::particle_count const &,
		flakes::particle_minimum_size const &,
		flakes::particle_maximum_size const &,
		flakes::snow_texture_size const &);

	void
	update(
		flakelib::duration const &,
		buffer::volume_view<cl_float4> const &);

	void
	render();

	sge::renderer::texture::planar_ptr const
	current_snow_texture();

	~object();
private:
	typedef
	fcppt::container::array<sge::renderer::texture::planar_ptr,2>
	planar_ptr_array;

	typedef
	boost::ptr_array<sge::opencl::memory_object::image::planar,2>
	cl_image_array;

	sge::renderer::device &renderer_;
	sge::camera::base &camera_;
	sge::opencl::command_queue::object &command_queue_;
	sge::renderer::vertex_declaration_ptr vertex_declaration_;
	sge::renderer::vertex_buffer_ptr vertex_buffer_;
	sge::shader::object shader_;
	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> cl_buffer_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object advect_kernel_;
	planar_ptr_array planar_ptrs_;
	cl_image_array cl_images_;
	std::size_t current_texture_;

	void
	generate_snow_textures(
		flakes::snow_texture_size const &);

	void
	generate_particles(
		flakes::particle_count const &,
		flakes::particle_minimum_size const &,
		flakes::particle_maximum_size const &,
		volume::grid_size const &);
};
}
}
}

#endif
