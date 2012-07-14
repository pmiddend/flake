#ifndef FLAKE_POSTPROCESSING_CONTEXT_HPP_INCLUDED
#define FLAKE_POSTPROCESSING_CONTEXT_HPP_INCLUDED

#include <sge/renderer/context/scoped_unique_ptr.hpp>
#include <flake/shader/parameter/planar_texture.hpp>
#include <flake/postprocessing/fullscreen_quad.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <sge/renderer/target/offscreen_scoped_ptr.hpp>
#include <sge/renderer/texture/planar_scoped_ptr.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <flake/shader/context_fwd.hpp>
#include <flake/shader/pair.hpp>

namespace flake
{
namespace postprocessing
{
class context
{
FCPPT_NONCOPYABLE(
	context);
public:
	context(
		sge::renderer::device &,
		sge::viewport::manager &,
		flake::shader::context &);

	sge::renderer::context::scoped_unique_ptr
	create_render_context();

	void
	render();

	~context();
private:
	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration_scoped_ptr const quad_vertex_declaration_;
	flake::postprocessing::fullscreen_quad fullscreen_quad_;
	flake::shader::pair downsample_shader_;
	flake::shader::pair finalize_shader_;
	/*
	flake::shader::pair blur_horizontal_shader_;
	flake::shader::pair blur_vertical_shader_;
	*/
	flake::shader::parameter::planar_texture input_texture_parameter_;
	fcppt::signal::scoped_connection viewport_connection_;
	sge::renderer::texture::planar_scoped_ptr rendering_result_texture_;
	sge::renderer::target::offscreen_scoped_ptr offscreen_target_;
	sge::renderer::texture::planar_scoped_ptr buffer_texture_0_;
	sge::renderer::texture::planar_scoped_ptr buffer_texture_1_;
	sge::renderer::texture::planar_scoped_ptr result_texture_;

	void
	viewport_callback();
};
}
}

#endif
