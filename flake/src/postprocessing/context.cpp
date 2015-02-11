#include <flake/media_path_from_string.hpp>
#include <flake/postprocessing/context.hpp>
#include <sge/image/ds/format.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/color_buffer/optional_surface_ref.hpp>
#include <sge/renderer/color_buffer/surface.hpp>
#include <sge/renderer/context/ffp.hpp>
#include <sge/renderer/context/scoped_ffp.hpp>
#include <sge/renderer/depth_stencil_buffer/optional_surface_ref.hpp>
#include <sge/renderer/depth_stencil_buffer/surface.hpp>
#include <sge/renderer/depth_stencil_buffer/surface_parameters.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/state/core/sampler/object.hpp>
#include <sge/renderer/state/core/sampler/parameters.hpp>
#include <sge/renderer/state/core/sampler/scoped.hpp>
#include <sge/renderer/state/core/sampler/address/mode_all.hpp>
#include <sge/renderer/state/core/sampler/filter/linear.hpp>
#include <sge/renderer/state/core/sampler/filter/point.hpp>
#include <sge/renderer/target/from_texture.hpp>
#include <sge/renderer/target/offscreen.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <sge/renderer/target/viewport.hpp>
#include <sge/renderer/target/viewport_size.hpp>
#include <sge/renderer/texture/capabilities_field.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vertex/declaration.hpp>
#include <sge/shader/scoped_pair.hpp>
#include <sge/viewport/manager.hpp>
#include <fcppt/make_cref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_map.hpp>
#include <fcppt/cast/size_fun.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/dim/to_signed.hpp>
#include <fcppt/signal/connection.hpp>
#include <fcppt/config/external_begin.hpp>
#include <functional>
#include <utility>
#include <fcppt/config/external_end.hpp>


flake::postprocessing::context::context(
	sge::renderer::device::ffp &_renderer,
	sge::viewport::manager &_viewport_manager,
	sge::shader::context &_shader_context)
:
	renderer_(
		_renderer),
	quad_vertex_declaration_(
		flake::postprocessing::fullscreen_quad::create_vertex_declaration(
			renderer_)),
	linear_clamping_texture_state_(
		renderer_.create_sampler_state(
			sge::renderer::state::core::sampler::parameters(
				sge::renderer::state::core::sampler::address::mode_all(
					sge::renderer::state::core::sampler::address::mode::clamp),
				sge::renderer::state::core::sampler::filter::linear()))),
	point_clamping_texture_state_(
		renderer_.create_sampler_state(
			sge::renderer::state::core::sampler::parameters(
				sge::renderer::state::core::sampler::address::mode_all(
					sge::renderer::state::core::sampler::address::mode::clamp),
				sge::renderer::state::core::sampler::filter::point()))),
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
				FCPPT_TEXT("shaders/postprocessing/downsample.cg"))),
		sge::shader::optional_cflags()),
	downsample_input_texture_parameter_(
		downsample_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"input_texture")),
		downsample_shader_,
		renderer_,
		sge::shader::parameter::planar_texture::optional_value()),
	blur_h_shader_(
		_shader_context,
		*quad_vertex_declaration_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/blur_h.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/blur_h.cg"))),
		sge::shader::optional_cflags()),
	blur_h_input_texture_parameter_(
		blur_h_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"input_texture")),
		blur_h_shader_,
		renderer_,
		sge::shader::parameter::planar_texture::optional_value()),
	blur_v_shader_(
		_shader_context,
		*quad_vertex_declaration_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/blur_v.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/blur_v.cg"))),
		sge::shader::optional_cflags()),
	blur_v_input_texture_parameter_(
		blur_v_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"input_texture")),
		blur_v_shader_,
		renderer_,
		sge::shader::parameter::planar_texture::optional_value()),
	finalize_shader_(
		_shader_context,
		*quad_vertex_declaration_,
		sge::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/finalize.cg"))),
		sge::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/postprocessing/finalize.cg"))),
		sge::shader::optional_cflags()),
	finalize_input_texture_parameter_(
		finalize_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"input_texture")),
		finalize_shader_,
		renderer_,
		sge::shader::parameter::planar_texture::optional_value()),
	finalize_blurred_texture_parameter_(
		finalize_shader_.pixel_program(),
		sge::shader::parameter::name(
			sge::cg::string(
				"blurred_texture")),
		finalize_shader_,
		renderer_,
		sge::shader::parameter::planar_texture::optional_value()),
	viewport_connection_(
		_viewport_manager.manage_callback(
			std::bind(
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

sge::renderer::context::scoped_ffp_unique_ptr
flake::postprocessing::context::create_render_context()
{
	FCPPT_ASSERT_PRE(
		rendering_result_texture_);

	FCPPT_ASSERT_PRE(
		offscreen_target_);

	this->switch_target_texture(
		*rendering_result_texture_);

	return
		fcppt::make_unique_ptr<sge::renderer::context::scoped_ffp>(
			renderer_,
			*offscreen_target_);
}

void
flake::postprocessing::context::render()
{
	this->render_and_return_overlay();
}

sge::renderer::context::scoped_ffp_unique_ptr
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
		fcppt::math::dim::structure_cast<
			sge::renderer::dim2,
			fcppt::cast::size_fun>(
			sge::renderer::target::viewport_size(
				renderer_.onscreen_target())));

	sge::renderer::size_type const downsample_factor =
		4u;

	downsampled_texture_0_ =
		renderer_.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				target_size / downsample_factor,
				sge::renderer::texture::color_format(
					sge::image::color::format::r32f,
					sge::renderer::texture::emulate_srgb::no),
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field::null(),
				sge::renderer::texture::capabilities_field{
					sge::renderer::texture::capabilities::render_target}));

	downsampled_texture_1_ =
		renderer_.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				target_size / downsample_factor,
				sge::renderer::texture::color_format(
					sge::image::color::format::r32f,
					sge::renderer::texture::emulate_srgb::no),
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field::null(),
				sge::renderer::texture::capabilities_field{
					sge::renderer::texture::capabilities::render_target}));

	rendering_result_texture_ =
		renderer_.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				target_size,
				sge::renderer::texture::color_format(
					sge::image::color::format::rgba32f,
					sge::renderer::texture::emulate_srgb::no),
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field::null(),
				sge::renderer::texture::capabilities_field{
					sge::renderer::texture::capabilities::render_target}));

	finalize_input_texture_parameter_.set(
		sge::shader::parameter::planar_texture::optional_value(
			*rendering_result_texture_));

	finalize_blurred_texture_parameter_.set(
		sge::shader::parameter::planar_texture::optional_value(
			*downsampled_texture_0_));

	offscreen_target_ =
		sge::renderer::target::from_texture(
			renderer_,
			*rendering_result_texture_);

	offscreen_downsampled_target_ =
		sge::renderer::target::from_texture(
			renderer_,
			*downsampled_texture_0_);

	depth_stencil_surface_ =
		renderer_.create_depth_stencil_surface(
			sge::renderer::depth_stencil_buffer::surface_parameters(
				target_size,
				sge::image::ds::format::d32));

	offscreen_target_->depth_stencil_surface(
		sge::renderer::depth_stencil_buffer::optional_surface_ref(
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
				fcppt::math::dim::structure_cast<
					sge::renderer::pixel_rect::dim,
					fcppt::cast::size_fun>(
					fcppt::math::dim::to_signed(
						_new_texture.size())))));
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
				fcppt::math::dim::structure_cast<
					sge::renderer::pixel_rect::dim,
					fcppt::cast::size_fun>(
					fcppt::math::dim::to_signed(
						_new_texture.size())))));
}

void
flake::postprocessing::context::downsample()
{
	this->switch_downsampled_target_texture(
		*downsampled_texture_0_);

	sge::renderer::context::scoped_ffp const scoped_block(
		renderer_,
		*offscreen_downsampled_target_);

	downsample_input_texture_parameter_.set(
		sge::shader::parameter::planar_texture::optional_value(
			*rendering_result_texture_));

	sge::shader::scoped_pair scoped_shader(
		scoped_block.get(),
		downsample_shader_);

	sge::renderer::state::core::sampler::scoped scoped_address_mode(
		scoped_block.get(),
		fcppt::assign::make_map<sge::renderer::state::core::sampler::const_object_ref_map>
			(
				downsample_input_texture_parameter_.stage(),
				fcppt::make_cref(
					*linear_clamping_texture_state_)));

	fullscreen_quad_.render(
		scoped_block.get());
}

void
flake::postprocessing::context::blur_h()
{
	this->switch_downsampled_target_texture(
		*downsampled_texture_1_);

	sge::renderer::context::scoped_ffp const scoped_block(
		renderer_,
		*offscreen_downsampled_target_);

	blur_h_input_texture_parameter_.set(
		sge::shader::parameter::planar_texture::optional_value(
			*downsampled_texture_0_));

	sge::shader::scoped_pair scoped_shader(
		scoped_block.get(),
		blur_h_shader_);

	sge::renderer::state::core::sampler::scoped scoped_address_mode(
		scoped_block.get(),
		fcppt::assign::make_map<sge::renderer::state::core::sampler::const_object_ref_map>
		(
			blur_h_input_texture_parameter_.stage(),
			fcppt::make_cref(
				   *point_clamping_texture_state_)));

	fullscreen_quad_.render(
		scoped_block.get());
}

void
flake::postprocessing::context::blur_v()
{
	this->switch_downsampled_target_texture(
		*downsampled_texture_0_);

	sge::renderer::context::scoped_ffp const scoped_block(
		renderer_,
		*offscreen_downsampled_target_);

	blur_v_input_texture_parameter_.set(
		sge::shader::parameter::planar_texture::optional_value(
			*downsampled_texture_1_));

	sge::shader::scoped_pair scoped_shader(
		scoped_block.get(),
		blur_v_shader_);

	sge::renderer::state::core::sampler::scoped scoped_address_mode(
		scoped_block.get(),
		fcppt::assign::make_map<sge::renderer::state::core::sampler::const_object_ref_map>
			(
				blur_v_input_texture_parameter_.stage(),
				fcppt::make_cref(
					*point_clamping_texture_state_)));

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

sge::renderer::context::scoped_ffp_unique_ptr
flake::postprocessing::context::finalize()
{
	sge::renderer::context::scoped_ffp_unique_ptr result(
		fcppt::make_unique_ptr<sge::renderer::context::scoped_ffp>(
			renderer_,
			renderer_.onscreen_target()));

	sge::shader::scoped_pair scoped_shader(
		result->get(),
		finalize_shader_);

	FCPPT_ASSERT_PRE(
		finalize_input_texture_parameter_.stage().get() == 0u);

	FCPPT_ASSERT_PRE(
		finalize_blurred_texture_parameter_.stage().get() == 1u);

	sge::renderer::state::core::sampler::scoped scoped_address_mode(
		result->get(),
		fcppt::assign::make_map<sge::renderer::state::core::sampler::const_object_ref_map>
			(
				finalize_input_texture_parameter_.stage(),
				fcppt::make_cref(
					*point_clamping_texture_state_))
			(
				finalize_blurred_texture_parameter_.stage(),
				fcppt::make_cref(
					*linear_clamping_texture_state_)));

	fullscreen_quad_.render(
		result->get());

	return
		std::move(
			result);
}

