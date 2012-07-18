#ifndef FLAKE_POSTPROCESSING_CONTEXT_HPP_INCLUDED
#define FLAKE_POSTPROCESSING_CONTEXT_HPP_INCLUDED

#include <flake/postprocessing/fullscreen_quad.hpp>
#include <flake/shader/context_fwd.hpp>
#include <flake/shader/pair.hpp>
#include <flake/shader/parameter/planar_texture.hpp>
#include <flake/shader/parameter/vector.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/renderer/vertex_declaration_scoped_ptr.hpp>
#include <sge/renderer/context/scoped_unique_ptr.hpp>
#include <sge/renderer/target/offscreen_scoped_ptr.hpp>
#include <sge/renderer/texture/planar_scoped_ptr.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/signal/scoped_connection.hpp>


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

	sge::renderer::context::scoped_unique_ptr
	render_and_return_overlay();

	~context();
private:
	sge::renderer::device &renderer_;
	sge::renderer::vertex_declaration_scoped_ptr const quad_vertex_declaration_;
	flake::postprocessing::fullscreen_quad fullscreen_quad_;
	flake::shader::pair downsample_shader_;
	flake::shader::parameter::planar_texture downsample_input_texture_parameter_;
	flake::shader::pair blur_h_shader_;
	flake::shader::parameter::planar_texture blur_h_input_texture_parameter_;
	flake::shader::pair blur_v_shader_;
	flake::shader::parameter::planar_texture blur_v_input_texture_parameter_;
	flake::shader::pair finalize_shader_;
	flake::shader::parameter::planar_texture finalize_input_texture_parameter_;
	flake::shader::parameter::planar_texture finalize_blurred_texture_parameter_;
	fcppt::signal::scoped_connection viewport_connection_;
	sge::renderer::texture::planar_scoped_ptr rendering_result_texture_;
	sge::renderer::target::offscreen_scoped_ptr offscreen_target_;
	sge::renderer::target::offscreen_scoped_ptr offscreen_downsampled_target_;
	sge::renderer::texture::planar_scoped_ptr downsampled_texture_0_;
	sge::renderer::texture::planar_scoped_ptr downsampled_texture_1_;

	void
	viewport_callback();

	void
	switch_target_texture(
		sge::renderer::texture::planar &);

	void
	switch_downsampled_target_texture(
		sge::renderer::texture::planar &);

	void
	downsample();

	void
	blur_h();

	void
	blur_v();

	void
	blur();

	sge::renderer::context::scoped_unique_ptr
	finalize();
};
}
}

#endif
