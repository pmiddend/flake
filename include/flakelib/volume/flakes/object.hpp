#ifndef FLAKELIB_VOLUME_FLAKES_OBJECT_HPP_INCLUDED
#define FLAKELIB_VOLUME_FLAKES_OBJECT_HPP_INCLUDED

#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/shader/object.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <flakelib/buffer/volume_view_fwd.hpp>
#include <flakelib/build_options.hpp>
#include <flakelib/volume/flakes/particle_count.hpp>
#include <flakelib/volume/flakes/grid_size.hpp>
#include <flakelib/volume/flakes/particle_minimum_size.hpp>
#include <flakelib/volume/flakes/particle_maximum_size.hpp>
#include <flakelib/duration.hpp>
#include <fcppt/scoped_ptr.hpp>

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
public:
	explicit
	object(
		sge::renderer::device &,
		sge::image2d::system &,
		sge::opencl::command_queue::object &,
		flakelib::build_options const &,
		flakes::particle_count const &,
		flakes::grid_size const &,
		flakes::particle_minimum_size const &,
		flakes::particle_maximum_size const &);

	void
	update(
		flakelib::duration const &,
		buffer::volume_view<cl_float4> const &);

	void
	render(
		sge::renderer::matrix4 const &);

	~object();
private:
	sge::renderer::device &renderer_;
	sge::opencl::command_queue::object &command_queue_;
	sge::renderer::vertex_declaration_ptr vertex_declaration_;
	sge::renderer::vertex_buffer_ptr vertex_buffer_;
	sge::shader::object shader_;
	fcppt::scoped_ptr<sge::opencl::memory_object::buffer> cl_buffer_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object advect_kernel_;
};
}
}
}

#endif
