#ifndef FLAKELIB_VOLUME_DENSITY_VISUAL_HPP_INCLUDED
#define FLAKELIB_VOLUME_DENSITY_VISUAL_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/volume/density/grid_size.hpp>
#include <flakelib/volume/boundary/view.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/filesystem/path.hpp>


namespace flakelib
{
namespace volume
{
namespace density
{
class visual
{
FCPPT_NONCOPYABLE(
	visual);
public:
	explicit
	visual(
		sge::renderer::device &,
		sge::image2d::system &,
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
		density::grid_size const &,
		boundary::view const &);

	void
	update(
		flakelib::buffer::volume_view<cl_float> const &);

	void
	render(
		sge::renderer::vector3 const &,
		sge::renderer::matrix4 const &);

	~visual();
private:
	sge::renderer::device &renderer_;
	sge::opencl::command_queue::object &command_queue_;
	sge::image2d::system &image_system_;
	boundary::view boundary_;
	// CL stuff
	sge::renderer::texture::planar_ptr boundary_texture_;
	sge::renderer::texture::planar_ptr texture_;
	sge::opencl::memory_object::image::planar boundary_cl_texture_;
	sge::opencl::memory_object::image::planar cl_texture_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object volume_image_to_planar_texture_kernel_;
	// GL stuff
	sge::renderer::vertex_declaration_ptr vd_;
	sge::renderer::vertex_buffer_ptr vb_;
	sge::shader::object shader_;

	void
	volume_image_to_planar_texture(
		flakelib::buffer::volume_view<cl_float> const &,
		sge::opencl::memory_object::image::planar &);

	void
	save_texture_to_file(
		sge::renderer::texture::planar &,
		fcppt::filesystem::path const &);
};
}
}
}

#endif
