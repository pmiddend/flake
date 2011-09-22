#ifndef FLAKE_VISUALIZATION_ARROW_HPP_INCLUDED
#define FLAKE_VISUALIZATION_ARROW_HPP_INCLUDED

#include "base.hpp"
#include "../boundary_view.hpp"
#include "dummy_sprite/system.hpp"
#include "dummy_sprite/object.hpp"
#include "../simulation/base_fwd.hpp"
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/vertex_buffer.hpp>
#include <sge/renderer/vertex_declaration_ptr.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/sprite/object.hpp>
#include <sge/sprite/external_system_impl.hpp>
#include <sge/shader/object.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <fcppt/noncopyable.hpp>

namespace flake
{
namespace visualization
{
class arrow
:
	public visualization::base
{
FCPPT_NONCOPYABLE(
	arrow);
public:
	explicit
	arrow(
		sge::opencl::context::object &,
		sge::opencl::command_queue::object &,
		sge::renderer::device &,
		simulation::base &,
		flake::boundary_view const &,
		sge::parse::json::object const &);

	void
	update(
		flake::duration const &);

	void
	render();

	sge::renderer::state::list const
	render_states() const;

	~arrow();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::renderer::device &renderer_;
	simulation::base &simulation_;
	sge::renderer::vertex_declaration_ptr vd_;
	sge::renderer::vertex_buffer_ptr vb_;
	sge::opencl::memory_object::vertex_buffer cl_vb_;
	sge::opencl::program::object transfer_program_;
	sge::opencl::kernel::object transfer_kernel_;
	sge::shader::object shader_;
	dummy_sprite::system sprite_system_;
	dummy_sprite::object sprite_object_;
};
}
}

#endif
