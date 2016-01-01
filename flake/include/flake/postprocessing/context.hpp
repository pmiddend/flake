#ifndef FLAKE_POSTPROCESSING_CONTEXT_HPP_INCLUDED
#define FLAKE_POSTPROCESSING_CONTEXT_HPP_INCLUDED

#include <flake/postprocessing/fullscreen_quad.hpp>
#include <sge/renderer/context/scoped_ffp_unique_ptr.hpp>
#include <sge/renderer/depth_stencil_buffer/surface_unique_ptr.hpp>
#include <sge/renderer/device/ffp_fwd.hpp>
#include <sge/renderer/state/core/sampler/object_unique_ptr.hpp>
#include <sge/renderer/target/offscreen_unique_ptr.hpp>
#include <sge/renderer/texture/planar_unique_ptr.hpp>
#include <sge/renderer/vertex/declaration_unique_ptr.hpp>
#include <sge/shader/context_fwd.hpp>
#include <sge/shader/pair.hpp>
#include <sge/shader/parameter/planar_texture.hpp>
#include <sge/shader/parameter/vector.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/optional/object.hpp>
#include <fcppt/signal/auto_connection.hpp>


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
		sge::renderer::device::ffp &,
		sge::viewport::manager &,
		sge::shader::context &);

	sge::renderer::context::scoped_ffp_unique_ptr
	create_render_context();

	void
	render();

	sge::renderer::context::scoped_ffp_unique_ptr
	render_and_return_overlay();

	~context();
private:
	sge::renderer::device::ffp &renderer_;
	sge::renderer::vertex::declaration_unique_ptr const quad_vertex_declaration_;
	sge::renderer::state::core::sampler::object_unique_ptr const linear_clamping_texture_state_;
	sge::renderer::state::core::sampler::object_unique_ptr const point_clamping_texture_state_;
	flake::postprocessing::fullscreen_quad fullscreen_quad_;
	sge::shader::pair downsample_shader_;
	sge::shader::parameter::planar_texture downsample_input_texture_parameter_;
	sge::shader::pair blur_h_shader_;
	sge::shader::parameter::planar_texture blur_h_input_texture_parameter_;
	sge::shader::pair blur_v_shader_;
	sge::shader::parameter::planar_texture blur_v_input_texture_parameter_;
	sge::shader::pair finalize_shader_;
	sge::shader::parameter::planar_texture finalize_input_texture_parameter_;
	sge::shader::parameter::planar_texture finalize_blurred_texture_parameter_;
	fcppt::signal::auto_connection viewport_connection_;
	fcppt::optional::object<
		sge::renderer::texture::planar_unique_ptr
	> rendering_result_texture_;
	fcppt::optional::object<
		sge::renderer::target::offscreen_unique_ptr
	> offscreen_target_;
	fcppt::optional::object<
		sge::renderer::depth_stencil_buffer::surface_unique_ptr
	> depth_stencil_surface_;
	typedef
	fcppt::optional::object<
		sge::renderer::target::offscreen_unique_ptr
	>
	optional_offscreen_target_ptr;
	optional_offscreen_target_ptr offscreen_downsampled_target_;
	typedef
	fcppt::optional::object<
		sge::renderer::texture::planar_unique_ptr
	>
	optional_planar_texture_ptr;
	optional_planar_texture_ptr downsampled_texture_0_;
	optional_planar_texture_ptr downsampled_texture_1_;

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

	sge::renderer::context::scoped_ffp_unique_ptr
	finalize();
};
}
}

#endif
