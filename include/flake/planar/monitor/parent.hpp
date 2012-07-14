#ifndef FLAKE_PLANAR_MONITOR_PARENT_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_PARENT_HPP_INCLUDED

#include <flake/sprite_drawer_3d.hpp>
#include <flake/planar/monitor/arrow_scale.hpp>
#include <flake/planar/monitor/child.hpp>
#include <flake/planar/monitor/child_list.hpp>
#include <flake/planar/monitor/font_color.hpp>
#include <flake/planar/monitor/grid_dimensions.hpp>
#include <flake/planar/monitor/grid_scale.hpp>
#include <flake/planar/monitor/optional_projection.hpp>
#include <flake/planar/monitor/scaling_factor.hpp>
#include <flake/planar/monitor/dummy_sprite/collection.hpp>
#include <flake/planar/monitor/dummy_sprite/system.hpp>
#include <flake/shader/context_fwd.hpp>
#include <flake/shader/pair.hpp>
#include <flake/shader/parameter/vector.hpp>
#include <flake/shader/parameter/matrix.hpp>
#include <sge/font/metrics_shared_ptr.hpp>
#include <sge/image/color/any/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/buffer_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_declaration_fwd.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <fcppt/noncopyable.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
/**
\details
This class serves two purposes:

<ol>
<li>
It's a parent class, having a number of children. The parent calls "render"
(and later probably "update") on all children.
</li>
<li>
It's a factory class for shaders/OpenCL objects. It keeps the context and the
vertex declaration as well as the sprite system.
</li>
</ol>
*/
class parent
{
FCPPT_NONCOPYABLE(
	parent);
public:
	parent(
		sge::renderer::device &,
		flake::shader::context &,
		sge::opencl::command_queue::object &,
		sge::font::metrics_shared_ptr,
		monitor::font_color const &);

	sge::renderer::vertex_declaration const &
	arrow_vertex_declaration() const;

	sge::opencl::context::object &
	cl_context() const;

	flake::shader::pair &
	arrow_shader();

	void
	arrow_projection(
		sge::renderer::matrix4 const &);

	void
	arrow_position(
		sge::renderer::vector2 const &);

	sge::renderer::device &
	renderer() const;

	monitor::dummy_sprite::collection &
	sprite_collection();

	sge::font::metrics &
	font_metrics();

	flake::sprite_drawer_3d &
	font_drawer();

	void
	render(
		sge::renderer::context::object &,
		monitor::optional_projection const &);

	void
	update();

	~parent();
private:
	friend class monitor::child;

	sge::renderer::device &renderer_;
	sge::opencl::command_queue::object &command_queue_;
	sge::font::metrics_shared_ptr font_metrics_;
	flake::sprite_drawer_3d font_drawer_;
	sge::renderer::vertex_declaration_scoped_ptr vd_;

	flake::shader::pair arrow_shader_;
	flake::shader::parameter::vector<sge::renderer::scalar,2> arrow_initial_position_parameter_;
	flake::shader::parameter::matrix<sge::renderer::scalar,4,4> arrow_projection_parameter_;

	monitor::dummy_sprite::system sprite_system_;
	monitor::dummy_sprite::collection sprite_collection_;
	monitor::child_list children_;

	void
	add_child(
		monitor::child &);

	void
	erase_child(
		monitor::child &);
};
}
}
}

#endif
