#include <flakelib/visualization/arrow.hpp>
#include <flakelib/simulation/base.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/image/colors.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/texture/create_planar_from_view.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/font/system.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/text.hpp>

flakelib::visualization::arrow::arrow(
	sge::renderer::device &_renderer,
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_command_queue,
	simulation::base &_simulation,
	sge::font::system &_font_system,
	flakelib::boundary_view const &_boundary,
	sge::parse::json::object const &_config_file)
:
	renderer_(
		_renderer),
	monitor_parent_(
		_renderer,
		_context,
		_command_queue,
		_font_system.create_font(
			flakelib::media_path_from_string(
				FCPPT_TEXT("fonts/main.ttf")),
			sge::parse::json::find_and_convert_member<sge::font::size_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("visualization/font-size")))),
		monitor::border_size(
			sge::parse::json::find_and_convert_member<monitor::border_size::value_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("visualization/border-size")))),
		monitor::font_color(
			sge::image::colors::black())),
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
					FCPPT_TEXT("visualization/arrow-scale")))),
		monitor::grid_scale(
			sge::parse::json::find_and_convert_member<monitor::arrow_scale::value_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("grid-scale")))),
		sge::renderer::texture::create_planar_from_view(
			renderer_,
			_boundary.get(),
			sge::renderer::texture::mipmap::off(),
			sge::renderer::texture::address_mode2(
				sge::renderer::texture::address_mode::clamp),
			sge::renderer::resource_flags::none)),
	pressure_(
		monitor_parent_,
		monitor::name(
			FCPPT_TEXT("pressure")),
		monitor::grid_dimensions(
			fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					_boundary.get()))),
		monitor::rect(
			velocity_arrows_.area().pos(),
			velocity_arrows_.area().size()/static_cast<monitor::rect::value_type>(4)),
		monitor::scaling_factor(
			sge::parse::json::find_and_convert_member<monitor::arrow_scale::value_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("visualization/pressure-scale"))))),
	velocity_magnitude_(
		monitor_parent_,
		monitor::name(
			FCPPT_TEXT("velocity absolute")),
		monitor::grid_dimensions(
			fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					_boundary.get()))),
		monitor::rect(
			velocity_arrows_.area().pos(),
			velocity_arrows_.area().size()/static_cast<monitor::rect::value_type>(4)),
		monitor::scaling_factor(1.00f)),
	divergence_(
		monitor_parent_,
		monitor::name(
			FCPPT_TEXT("divergence")),
		monitor::grid_dimensions(
			fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					_boundary.get()))),
		monitor::rect(
			velocity_arrows_.area().pos(),
			velocity_arrows_.area().size()/static_cast<monitor::rect::value_type>(4)),
		monitor::scaling_factor(
			sge::parse::json::find_and_convert_member<monitor::rect::value_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("visualization/divergence-scale"))))),
	simulation_(
		_simulation)
{
}

void
flakelib::visualization::arrow::update(
	flakelib::duration const &)
{
	velocity_arrows_.from_planar_object(
		simulation_.velocity());
	pressure_.from_planar_object(
		simulation_.pressure());
	velocity_magnitude_.from_planar_object(
		simulation_.velocity_magnitude());
	divergence_.from_planar_object(
		simulation_.divergence());
	monitor_parent_.update();
}

void
flakelib::visualization::arrow::render()
{
	monitor_parent_.render();
}

sge::renderer::state::list const
flakelib::visualization::arrow::render_states() const
{
	return
		sge::renderer::state::list
			(sge::renderer::state::bool_::clear_back_buffer = true)
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::paleturquoise());
}

flakelib::visualization::arrow::~arrow()
{
}
