#include <flakelib/arrow_visual.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/simulation/base.hpp>
#include <sge/font/system.hpp>
#include <sge/image/colors.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/create_planar_from_view.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <fcppt/text.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>


flakelib::arrow_visual::arrow_visual(
	simulation::base &_simulation,
	sge::renderer::device &_renderer,
	sge::opencl::command_queue::object &_command_queue,
	sge::font::system &_font_system,
	sge::parse::json::object const &_config_file,
	flakelib::build_options const &_build_options,
	sge::viewport::manager &_viewport_manager,
	flakelib::boundary_view const &_boundary)
:
	simulation_(
		_simulation),
	monitor_parent_(
		_renderer,
		_command_queue,
		_font_system.create_font(
			flakelib::media_path_from_string(
				FCPPT_TEXT("fonts/main.ttf")),
			sge::parse::json::find_and_convert_member<sge::font::size_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("font-size")))),
		monitor::font_color(
			sge::image::colors::black())),
	planar_converter_(
		_command_queue,
		_build_options),
	viewport_widget_(
		_viewport_manager,
		_renderer),
	velocity_arrows_(
		monitor_parent_,
		monitor::name(
			FCPPT_TEXT("velocity")),
		// Take the grid dimensions from the boundary image
		monitor::grid_dimensions(
			fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					_boundary.get()))),
		monitor::arrow_scale(
			sge::parse::json::find_and_convert_member<monitor::arrow_scale::value_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("arrow-scale")))),
		monitor::grid_scale(
			sge::parse::json::find_and_convert_member<monitor::arrow_scale::value_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("grid-scale")))),
		sge::renderer::texture::create_planar_from_view(
			_renderer,
			_boundary.get(),
			sge::renderer::texture::mipmap::off(),
			sge::renderer::texture::address_mode2(
				sge::renderer::texture::address_mode::clamp),
			sge::renderer::resource_flags_field(
				sge::renderer::resource_flags::none)))
{
	viewport_widget_.child(
		velocity_arrows_.widget());
}

void
flakelib::arrow_visual::update(
	flakelib::duration const &_dt)
{
	planar_converter_.to_vb(
		flakelib::planar_object(
			simulation_.velocity()),
		velocity_arrows_.cl_buffer(),
		monitor::grid_scale(
			velocity_arrows_.grid_scale()),
		monitor::arrow_scale(
			velocity_arrows_.arrow_scale()));

	monitor_parent_.update();
}

void
flakelib::arrow_visual::render()
{
	monitor_parent_.render(
		monitor::optional_projection());
}

flakelib::arrow_visual::~arrow_visual()
{
}
