#ifndef FLAKE_POSTPROCESSING_FULLSCREEN_QUAD_HPP_INCLUDED
#define FLAKE_POSTPROCESSING_FULLSCREEN_QUAD_HPP_INCLUDED

#include <sge/renderer/device/core_fwd.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/vertex_declaration_fwd.hpp>
#include <sge/renderer/vertex_declaration_unique_ptr.hpp>
#include <sge/renderer/context/core.hpp>
#include <fcppt/noncopyable.hpp>


namespace flake
{
namespace postprocessing
{
class fullscreen_quad
{
FCPPT_NONCOPYABLE(
	fullscreen_quad);
public:
	fullscreen_quad(
		sge::renderer::device::core &,
		sge::renderer::vertex_declaration &);

	void
	render(
		sge::renderer::context::core &);

	~fullscreen_quad();

	static
	sge::renderer::vertex_declaration_unique_ptr
	create_vertex_declaration(
		sge::renderer::device::core &);
private:
	sge::renderer::device::core &renderer_;
	sge::renderer::vertex_declaration &vertex_declaration_;
	sge::renderer::vertex_buffer_scoped_ptr const vertex_buffer_;
};
}
}

#endif
