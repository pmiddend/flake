#include <flakelib/media_path_from_string.hpp>
#include <flakelib/planar_framework.hpp>
#include <flakelib/monitor/texture.hpp>
#include <flakelib/simulation/base.hpp>
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


flakelib::planar_framework::planar_framework(
	sge::viewport::manager &_viewport_manager,
	sge::renderer::device &_renderer,
	sge::opencl::command_queue::object &_command_queue,
	simulation::base &_simulation,
	sge::font::system &_font_system,
	flakelib::build_options const &_build_options,
	flakelib::boundary_view const &_boundary,
	sge::parse::json::object const &_config_file,
	sge::input::cursor::object &_cursor,
	planar_pool::object &_scalar_pool,
	utility::object &_utility)
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
	master_and_slaves_(
		rucksack::padding(
			sge::parse::json::find_and_convert_member<rucksack::scalar>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("border-size"))))),
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
			sge::renderer::texture::address_mode2(
				sge::renderer::texture::address_mode::clamp),
			sge::renderer::resource_flags_field(
				sge::renderer::resource_flags::none))),
	density_advector_(
		_command_queue,
		_scalar_pool,
		_utility,
		_build_options,
		density::grid_dimensions(
			fcppt::math::dim::structure_cast<density::grid_dimensions::value_type>(
				sge::image2d::view::size(
					_boundary.get()))),
		density::grid_scale(
			sge::parse::json::find_and_convert_member<monitor::arrow_scale::value_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("grid-scale"))))),
	density_cursor_splatter_(
		_command_queue,
		density::source_image(
			density_advector_.source_image()),
		_cursor,
		renderer_,
		_build_options,
		density::splat_radius(
			sge::parse::json::find_and_convert_member<density::splat_radius::value_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("density-splat-radius"))))),
	density_monitor_(
		monitor_parent_,
		monitor::grid_dimensions(
			fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
				sge::image2d::view::size(
					_boundary.get()))),
		monitor::rect::dim(
			static_cast<monitor::scalar>(
				velocity_arrows_.widget().axis_policy().x().minimum_size()),
			static_cast<monitor::scalar>(
				velocity_arrows_.widget().axis_policy().y().minimum_size())),
		planar_converter_),
	additional_data_()
{
	viewport_widget_.child(
		master_and_slaves_);

	master_and_slaves_.master_pane(
		master_box_);

	master_box_.push_back_child(
		velocity_arrows_.widget(),
		rucksack::alignment::left_or_top);

	master_box_.push_back_child(
		density_monitor_.monitor().widget(),
		rucksack::alignment::left_or_top);

	for(
		flakelib::additional_planar_data::const_iterator it =
			simulation_.additional_planar_data().begin();
		it !=simulation_.additional_planar_data().end();
		++it)
	{
		fcppt::container::ptr::insert_unique_ptr_map(
			additional_data_,
			it->key(),
			fcppt::make_unique_ptr<monitor::texture>(
				fcppt::ref(
					monitor_parent_),
				monitor::name(
					it->key()),
				monitor::grid_dimensions(
					fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
						sge::image2d::view::size(
							_boundary.get()))),
				monitor::rect::dim(
					static_cast<monitor::scalar>(
						velocity_arrows_.widget().axis_policy().x().minimum_size()/static_cast<rucksack::scalar>(4)),
					static_cast<monitor::scalar>(
						velocity_arrows_.widget().axis_policy().y().minimum_size()/static_cast<rucksack::scalar>(4))),
				monitor::scaling_factor(
					sge::parse::json::find_and_convert_member<monitor::scalar>(
						_config_file,
						sge::parse::json::string_to_path(
							it->key()+FCPPT_TEXT("-scale"))))));

		master_and_slaves_.push_back_child(
			additional_data_.find(it->key())->second->widget());
	}
}

void
flakelib::planar_framework::update(
	flakelib::duration const &_dt)
{
	density_cursor_splatter_.update_cursor_rectangle(
		density_monitor_.rectangle());

	density_advector_.update(
		density::velocity_image(
			*simulation_.velocity().get<sge::opencl::memory_object::image::planar *>()),
		_dt);

	density_monitor_.update(
		density_advector_.density_image());

	planar_converter_.to_vb(
		flakelib::planar_object(
			simulation_.velocity()),
		velocity_arrows_.cl_buffer(),
		monitor::grid_scale(
			velocity_arrows_.grid_scale()),
		monitor::arrow_scale(
			velocity_arrows_.arrow_scale()));

	for(
		flakelib::additional_planar_data::const_iterator it =
			simulation_.additional_planar_data().begin();
		it !=simulation_.additional_planar_data().end();
		++it)
	{
		for(additional_data_monitors::iterator it2 =
			additional_data_.begin();
			it2 != additional_data_.end();
			++it2)
		{
			if(it->key() != it2->first)
				continue;

			planar_converter_.to_texture(
				it->value(),
				it2->second->cl_texture(),
				monitor::scaling_factor(
					it2->second->scaling_factor()));
		}
	}

	monitor_parent_.update();
}

void
flakelib::planar_framework::render()
{
	monitor_parent_.render(
		monitor::optional_projection());
}

sge::renderer::state::list const
flakelib::planar_framework::render_states() const
{
	return
		sge::renderer::state::list
			(sge::renderer::state::bool_::clear_back_buffer = true)
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::paleturquoise());
}

flakelib::planar_framework::~planar_framework()
{
}
