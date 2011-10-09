#ifndef FLAKELIB_VISUALIZATION_MONITOR_PARENT_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_PARENT_HPP_INCLUDED

#include <flakelib/planar_object.hpp>
#include <flakelib/visualization/monitor/grid_dimensions.hpp>
#include <flakelib/visualization/monitor/dummy_sprite/system.hpp>
#include <flakelib/visualization/monitor/base.hpp>
#include <flakelib/visualization/monitor/arrow_scale.hpp>
#include <flakelib/visualization/monitor/scaling_factor.hpp>
#include <flakelib/visualization/monitor/grid_scale.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_declaration_fwd.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/sprite/intrusive/system_impl.hpp>
#include <sge/shader/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/intrusive/list.hpp>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace visualization
{
namespace monitor
{
/**
 * This class serves two purposes:
 *
 * 1. It's a parent class, having a number of children. The parent calls
 *    "render" (and later probably "update") on all children.
 *
 * 2. It's a factory class for shaders/OpenCL objects. It keeps the context and
 *    the vertex declaration as well as the sprite system.
 */
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
		flakelib::planar_object const &,
		sge::opencl::memory_object::buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	to_texture(
		flakelib::planar_object const &,
		sge::opencl::memory_object::image::planar &,
		monitor::scaling_factor const &);

	sge::shader::object &
	arrow_shader();

	sge::renderer::device &
	renderer() const;

	monitor::dummy_sprite::system &
	sprite_system();

	void
	render();

	~parent();
private:
	friend class monitor::base;

	typedef
	boost::intrusive::list
	<
		monitor::base,
		boost::intrusive::constant_time_size<false>
	>
	child_list;

	sge::renderer::device &renderer_;
	sge::opencl::context::object &context_;
	sge::opencl::command_queue::object &command_queue_;
	sge::renderer::vertex_declaration_ptr vd_;
	sge::opencl::program::object conversion_program_;
	sge::opencl::kernel::object image_to_vb_kernel_;
	sge::opencl::kernel::object buffer_to_vb_kernel_;
	sge::opencl::kernel::object image_to_image_kernel_;
	sge::opencl::kernel::object buffer_to_image_kernel_;
	sge::shader::object arrow_shader_;
	monitor::dummy_sprite::system sprite_system_;
	child_list children_;

	void
	image_to_vb(
		sge::opencl::memory_object::image::planar &,
		sge::opencl::memory_object::buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	planar_buffer_to_vb(
		flakelib::planar_buffer const &,
		sge::opencl::memory_object::buffer &,
		monitor::grid_scale const &,
		monitor::arrow_scale const &);

	void
	image_to_image(
		sge::opencl::memory_object::image::planar &,
		sge::opencl::memory_object::image::planar &,
		monitor::scaling_factor const &);

	void
	planar_buffer_to_image(
		flakelib::planar_buffer const &,
		sge::opencl::memory_object::image::planar &,
		monitor::scaling_factor const &);

	void
	add_child(
		monitor::base &);

	void
	erase_child(
		monitor::base &);
};
}
}
}

#endif
