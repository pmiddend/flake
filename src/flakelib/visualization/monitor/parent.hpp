#ifndef FLAKELIB_VISUALIZATION_MONITOR_PARENT_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_PARENT_HPP_INCLUDED

#include <flakelib/buffer_or_image.hpp>
#include <flakelib/visualization/monitor/grid_dimensions.hpp>
#include <flakelib/visualization/monitor/arrow_scale.hpp>
#include <flakelib/visualization/monitor/scaling_factor.hpp>
#include <flakelib/visualization/monitor/grid_scale.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_declaration_fwd.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/opencl/memory_object/vertex_buffer_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
class parent
{
FCPPT_NONCOPYABLE(
	parent);
public:
	explicit
	parent(
		sge::renderer::device &,
		sge::opencl::context::object &,
		sge::opencl::command_queue::object &);
	
	sge::renderer::vertex_declaration const &
	vertex_declaration() const;

	sge::opencl::context::object &
	context() const;

	void
	to_vb(
		flakelib::buffer_or_image const &,
		sge::opencl::memory_object::vertex_buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	to_texture(
		flakelib::buffer_or_image const &,
		sge::renderer::texture::planar &,
		monitor::scaling_factor const &);

	sge::shader::object &
	arrow_shader();

	sge::renderer::device &
	renderer() const;

	~parent();
private:
	sge::renderer::device &renderer_;
	sge::opencl::context::object &context_;
	sge::opencl::command_queue::object &command_queue_;
	sge::renderer::vertex_declaration_ptr vd_;
	sge::opencl::program::object conversion_program_;
	sge::opencl::kernel::object image_to_vb_kernel_;
	sge::opencl::kernel::object buffer_to_vb_kernel_;
	sge::shader::object arrow_shader_;

	void
	image_to_vb(
		sge::opencl::memory_object::image::planar &,
		sge::opencl::memory_object::vertex_buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	planar_buffer_to_vb(
		flakelib::planar_buffer const &,
		sge::opencl::memory_object::vertex_buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);
};
}
}
}

#endif
