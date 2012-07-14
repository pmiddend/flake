#include <flake/postprocessing/context.hpp>
#include <sge/renderer/depth_stencil_surface.hpp>
#include <sge/renderer/color_surface.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/target/viewport_size.hpp>
#include <sge/viewport/manager.hpp>
#include <flake/media_path_from_string.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <sge/renderer/texture/filter/point.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/target/viewport.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/signal/connection.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <fcppt/tr1/functional.hpp>
#include <sge/renderer/target/offscreen.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/context/scoped.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/context/object.hpp>
#include <sge/renderer/context/scoped.hpp>
#include <flake/shader/scoped_pair.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <sge/renderer/texture/capabilities_field.hpp>
#include <fcppt/optional_impl.hpp>
#include <sge/renderer/target/viewport_size.hpp>

flake::postprocessing::context::context(
	sge::renderer::device &_renderer,
	sge::viewport::manager &_viewport_manager,
	flake::shader::context &_shader_context)
:
	renderer_(
		_renderer),
	quad_vertex_declaration_(
		flake::postprocessing::fullscreen_quad::create_vertex_declaration(
			renderer_)),
	fullscreen_quad_(
		renderer_,
		*quad_vertex_declaration_),
	downsample_shader_(
		_shader_context,
		*quad_vertex_declaration_,
		flake::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/finalize.cg"))),
				//				FCPPT_TEXT("shaders/postprocessing/downsample.cg"))),
		flake::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/finalize.cg")))),
				//				FCPPT_TEXT("shaders/postprocessing/downsample.cg")))),
	/*
	blur_horizontal_shader_(),
	blur_vertical_shader_(),
	*/
	finalize_shader_(
		_shader_context,
		*quad_vertex_declaration_,
		flake::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/finalize.cg"))),
		flake::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/finalize.cg")))),
	input_texture_parameter_(
		finalize_shader_,
		renderer_,
		finalize_shader_.pixel_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"input_texture")),
		flake::shader::parameter::planar_texture::optional_value()),
	viewport_connection_(
		_viewport_manager.manage_callback(
			std::tr1::bind(
				&flake::postprocessing::context::viewport_callback,
				this))),
	rendering_result_texture_(),
	offscreen_target_(
		renderer_.create_target()),
	buffer_texture_0_(),
	buffer_texture_1_(),
	result_texture_()
{
}

sge::renderer::context::scoped_unique_ptr
flake::postprocessing::context::create_render_context()
{
	FCPPT_ASSERT_PRE(
		rendering_result_texture_);

	FCPPT_ASSERT_PRE(
		offscreen_target_);

	offscreen_target_->color_surface(
		sge::renderer::color_surface_shared_ptr(
			rendering_result_texture_->surface(
				sge::renderer::texture::mipmap::level(
					0u))),
		sge::renderer::target::surface_index(
			0u));

	return
		fcppt::make_unique_ptr<sge::renderer::context::scoped>(
			fcppt::ref(
				renderer_),
			fcppt::ref(
				*offscreen_target_));
}

void
flake::postprocessing::context::render()
{
	sge::renderer::context::scoped const scoped_block(
		renderer_,
		renderer_.onscreen_target());

	flake::shader::scoped_pair scoped_shader(
		scoped_block.get(),
		finalize_shader_);

	sge::renderer::texture::filter::scoped scoped_texture_filter(
		scoped_block.get(),
		input_texture_parameter_.stage(),
		sge::renderer::texture::filter::point());

	fullscreen_quad_.render(
		scoped_block.get());
}

flake::postprocessing::context::~context()
{
}

void
flake::postprocessing::context::viewport_callback()
{
	sge::renderer::dim2 const target_size(
		fcppt::math::dim::structure_cast<sge::renderer::dim2>(
			sge::renderer::target::viewport_size(
				renderer_.onscreen_target())));

	rendering_result_texture_.take(
		renderer_.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				target_size,
				sge::image::color::format::rgba32f,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags::none,
				sge::renderer::texture::capabilities_field(
					sge::renderer::texture::capabilities::render_target))));

	input_texture_parameter_.set(
		*rendering_result_texture_);

	offscreen_target_->depth_stencil_surface(
		sge::renderer::depth_stencil_surface_shared_ptr(
			renderer_.create_depth_stencil_surface(
				target_size,
				sge::renderer::depth_stencil_format::d32)));

	offscreen_target_->viewport(
		sge::renderer::target::viewport(
			sge::renderer::pixel_rect(
				sge::renderer::pixel_rect::vector::null(),
				fcppt::math::dim::structure_cast<sge::renderer::pixel_rect::dim>(
					target_size))));
}
