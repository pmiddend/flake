#include <flakelib/media_path_from_string.hpp>
#include <flakelib/planar/arrows_only.hpp>
#include <flakelib/planar/monitor/texture.hpp>
#include <flakelib/planar/simulation/base.hpp>
#include <sge/font/system.hpp>
#include <sge/image/colors.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/create_planar_from_view.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/container/ptr/insert_unique_ptr_map.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/basic_impl.hpp>


flakelib::planar::arrows_only::arrows_only(
	sge::viewport::manager &_viewport_manager,
	sge::renderer::device &_renderer,
	sge::opencl::command_queue::object &_command_queue,
	simulation::base &_simulation,
	sge::font::system &_font_system,
	flakelib::build_options const &_build_options,
	flakelib::planar::boundary_view const &_boundary,
	sge::parse::json::object const &_config_file)
:
	renderer_(
		_renderer),
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
	master_box_(
		rucksack::axis::y,
		rucksack::aspect(
			1,
			1)),
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
			renderer_,
			_boundary.get(),
			sge::renderer::texture::mipmap::off(),
			sge::renderer::resource_flags_field(
				sge::renderer::resource_flags::none)))
{
	viewport_widget_.child(
		master_box_);

	master_box_.push_back_child(
		velocity_arrows_.widget(),
		rucksack::alignment::center);
}

void
flakelib::planar::arrows_only::update(
	flakelib::duration const &)
{
	planar_converter_.to_arrow_vb(
		simulation_.velocity(),
		velocity_arrows_.cl_buffer(),
		monitor::grid_scale(
			velocity_arrows_.grid_scale()),
		monitor::arrow_scale(
			velocity_arrows_.arrow_scale()));

	monitor_parent_.update();
}

void
flakelib::planar::arrows_only::render(
	sge::renderer::matrix4 const &_mvp)
{
	monitor_parent_.render(
		monitor::optional_projection(
			_mvp));
}

sge::renderer::state::list const
flakelib::planar::arrows_only::render_states() const
{
	return
		sge::renderer::state::list
			(sge::renderer::state::bool_::clear_back_buffer = true)
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::grey());
}

flakelib::planar::arrows_only::~arrows_only()
{
}
