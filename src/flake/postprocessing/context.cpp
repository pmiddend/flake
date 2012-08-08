#include <flake/media_path_from_string.hpp>
#include <flake/postprocessing/context.hpp>
#include <sge/renderer/depth_stencil_surface.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/context/object.hpp>
#include <sge/renderer/context/scoped.hpp>
#include <sge/renderer/target/from_texture.hpp>
#include <sge/renderer/target/offscreen.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <sge/renderer/target/viewport.hpp>
#include <sge/renderer/target/viewport_size.hpp>
#include <sge/renderer/texture/address_mode.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/capabilities_field.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/set_address_mode2.hpp>
#include <sge/renderer/texture/filter/linear.hpp>
#include <sge/renderer/texture/filter/point.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/shader/scoped_pair.hpp>
#include <sge/viewport/manager.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/move.hpp>
#include <fcppt/optional_impl.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/signal/connection.hpp>
#include <fcppt/tr1/functional.hpp>


flake::postprocessing::context::context(
	sge::renderer::device &_renderer,
	sge::viewport::manager &_viewport_manager,
	sge::shader::context &_shader_context)
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
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/downsample.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/downsample.cg")))),
	downsample_input_texture_parameter_(
		downsample_shader_,
		renderer_,
		downsample_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"input_texture")),
		sge::shader::parameter::planar_texture::optional_value()),
	blur_h_shader_(
		_shader_context,
		*quad_vertex_declaration_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/blur_h.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/blur_h.cg")))),
	blur_h_input_texture_parameter_(
		blur_h_shader_,
		renderer_,
		blur_h_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"input_texture")),
		sge::shader::parameter::planar_texture::optional_value()),
	blur_v_shader_(
		_shader_context,
		*quad_vertex_declaration_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/blur_v.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/blur_v.cg")))),
	blur_v_input_texture_parameter_(
		blur_v_shader_,
		renderer_,
		blur_v_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"input_texture")),
		sge::shader::parameter::planar_texture::optional_value()),
	finalize_shader_(
		_shader_context,
		*quad_vertex_declaration_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/finalize.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/finalize.cg")))),
	finalize_input_texture_parameter_(
		finalize_shader_,
		renderer_,
		finalize_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"input_texture")),
		sge::shader::parameter::planar_texture::optional_value()),
	finalize_blurred_texture_parameter_(
		finalize_shader_,
		renderer_,
		finalize_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"blurred_texture")),
		sge::shader::parameter::planar_texture::optional_value()),
	viewport_connection_(
		_viewport_manager.manage_callback(
			std::tr1::bind(
				&flake::postprocessing::context::viewport_callback,
				this))),
	rendering_result_texture_(),
	offscreen_target_(),
	depth_stencil_surface_(),
	offscreen_downsampled_target_(),
	downsampled_texture_0_(),
	downsampled_texture_1_()
{
}

sge::renderer::context::scoped_unique_ptr
flake::postprocessing::context::create_render_context()
{
	FCPPT_ASSERT_PRE(
		rendering_result_texture_);

	FCPPT_ASSERT_PRE(
		offscreen_target_);

	this->switch_target_texture(
		*rendering_result_texture_);

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
	this->render_and_return_overlay();
}

sge::renderer::context::scoped_unique_ptr
flake::postprocessing::context::render_and_return_overlay()
{
	this->downsample();
	this->blur_h();
	this->blur_v();
	return
		this->finalize();
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

	sge::renderer::size_type const downsample_factor =
		4u;

	downsampled_texture_0_.take(
		renderer_.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				target_size / downsample_factor,
				sge::image::color::format::r32f,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field::null(),
				sge::renderer::texture::capabilities_field(
					sge::renderer::texture::capabilities::render_target))));

	downsampled_texture_1_.take(
		renderer_.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				target_size / downsample_factor,
				sge::image::color::format::r32f,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field::null(),
				sge::renderer::texture::capabilities_field(
					sge::renderer::texture::capabilities::render_target))));

	rendering_result_texture_.take(
		renderer_.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				target_size,
				sge::image::color::format::rgba32f,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field::null(),
				sge::renderer::texture::capabilities_field(
					sge::renderer::texture::capabilities::render_target))));

	finalize_input_texture_parameter_.set(
		*rendering_result_texture_);

	finalize_blurred_texture_parameter_.set(
		*downsampled_texture_0_);

	offscreen_target_.take(
		sge::renderer::target::from_texture(
			renderer_,
			*rendering_result_texture_));

	offscreen_downsampled_target_.take(
		sge::renderer::target::from_texture(
			renderer_,
			*downsampled_texture_0_));

	depth_stencil_surface_.take(
		renderer_.create_depth_stencil_surface(
			target_size,
			sge::renderer::depth_stencil_format::d32));

	offscreen_target_->depth_stencil_surface(
		sge::renderer::optional_depth_stencil_surface_ref(
			*depth_stencil_surface_));
}

void
flake::postprocessing::context::switch_downsampled_target_texture(
	sge::renderer::texture::planar &_new_texture)
{
	offscreen_downsampled_target_->color_surface(
		sge::renderer::color_buffer::optional_surface_ref(
			_new_texture.level(
				sge::renderer::texture::mipmap::level(
					0u))),
		sge::renderer::target::surface_index(
			0u));

	offscreen_downsampled_target_->viewport(
		sge::renderer::target::viewport(
			sge::renderer::pixel_rect(
				sge::renderer::pixel_rect::vector::null(),
				fcppt::math::dim::structure_cast<sge::renderer::pixel_rect::dim>(
					_new_texture.size()))));
}

void
flake::postprocessing::context::switch_target_texture(
	sge::renderer::texture::planar &_new_texture)
{
	offscreen_target_->color_surface(
		sge::renderer::color_buffer::optional_surface_ref(
			_new_texture.level(
				sge::renderer::texture::mipmap::level(
					0u))),
		sge::renderer::target::surface_index(
			0u));

	offscreen_target_->viewport(
		sge::renderer::target::viewport(
			sge::renderer::pixel_rect(
				sge::renderer::pixel_rect::vector::null(),
				fcppt::math::dim::structure_cast<sge::renderer::pixel_rect::dim>(
					_new_texture.size()))));
}

void
flake::postprocessing::context::downsample()
{
	this->switch_downsampled_target_texture(
		*downsampled_texture_0_);

	sge::renderer::context::scoped const scoped_block(
		renderer_,
		*offscreen_downsampled_target_);

	downsample_input_texture_parameter_.set(
		*rendering_result_texture_);

	sge::shader::scoped_pair scoped_shader(
		scoped_block.get(),
		downsample_shader_);

	sge::renderer::texture::filter::scoped scoped_texture_filter(
		scoped_block.get(),
		downsample_input_texture_parameter_.stage(),
		sge::renderer::texture::filter::linear());

	fullscreen_quad_.render(
		scoped_block.get());
}

void
flake::postprocessing::context::blur_h()
{
	this->switch_downsampled_target_texture(
		*downsampled_texture_1_);

	sge::renderer::context::scoped const scoped_block(
		renderer_,
		*offscreen_downsampled_target_);

	blur_h_input_texture_parameter_.set(
		*downsampled_texture_0_);

	sge::shader::scoped_pair scoped_shader(
		scoped_block.get(),
		blur_h_shader_);

	sge::renderer::texture::set_address_mode2(
		scoped_block.get(),
		blur_h_input_texture_parameter_.stage(),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::clamp));

	sge::renderer::texture::filter::scoped scoped_texture_filter(
		scoped_block.get(),
		blur_h_input_texture_parameter_.stage(),
		sge::renderer::texture::filter::point());

	fullscreen_quad_.render(
		scoped_block.get());
}

void
flake::postprocessing::context::blur_v()
{
	this->switch_downsampled_target_texture(
		*downsampled_texture_0_);

	sge::renderer::context::scoped const scoped_block(
		renderer_,
		*offscreen_downsampled_target_);

	blur_v_input_texture_parameter_.set(
		*downsampled_texture_1_);

	sge::shader::scoped_pair scoped_shader(
		scoped_block.get(),
		blur_v_shader_);

	sge::renderer::texture::set_address_mode2(
		scoped_block.get(),
		blur_v_input_texture_parameter_.stage(),
		sge::renderer::texture::address_mode2(
			sge::renderer::texture::address_mode::clamp));

	sge::renderer::texture::filter::scoped scoped_texture_filter(
		scoped_block.get(),
		blur_v_input_texture_parameter_.stage(),
		sge::renderer::texture::filter::point());

	fullscreen_quad_.render(
		scoped_block.get());
}

void
flake::postprocessing::context::blur()
{
	unsigned const blur_iterations = 2;

	for(unsigned i = 0; i < blur_iterations; ++i)
	{
		this->blur_h();
		this->blur_v();
	}
}

sge::renderer::context::scoped_unique_ptr
flake::postprocessing::context::finalize()
{
	sge::renderer::context::scoped_unique_ptr result(
		fcppt::make_unique_ptr<sge::renderer::context::scoped>(
			fcppt::ref(
				renderer_),
			fcppt::ref(
				renderer_.onscreen_target())));

	sge::shader::scoped_pair scoped_shader(
		result->get(),
		finalize_shader_);

	sge::renderer::texture::filter::scoped scoped_texture_filter_0(
		result->get(),
		finalize_input_texture_parameter_.stage(),
		sge::renderer::texture::filter::point());

	sge::renderer::texture::filter::scoped scoped_texture_filter_1(
		result->get(),
		finalize_blurred_texture_parameter_.stage(),
		sge::renderer::texture::filter::linear());

	fullscreen_quad_.render(
		result->get());

	return
		fcppt::move(
			result);
}

