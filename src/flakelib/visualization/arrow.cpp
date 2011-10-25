#include <flakelib/media_path_from_string.hpp>
#include <flakelib/simulation/base.hpp>
#include <flakelib/visualization/arrow.hpp>
#include <flakelib/visualization/monitor/texture.hpp>
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


flakelib::visualization::arrow::arrow(
	sge::viewport::manager &_viewport_manager,
	sge::renderer::device &_renderer,
	sge::opencl::command_queue::object &_command_queue,
	simulation::base &_simulation,
	sge::font::system &_font_system,
	flakelib::boundary_view const &_boundary,
	sge::parse::json::object const &_config_file)
:
	renderer_(
		_renderer),
	simulation_(
		_simulation),
	monitor_parent_(
		_viewport_manager,
		_renderer,
		_command_queue,
		_font_system.create_font(
			flakelib::media_path_from_string(
				FCPPT_TEXT("fonts/main.ttf")),
			sge::parse::json::find_and_convert_member<sge::font::size_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("visualization/font-size")))),
		monitor::font_color(
			sge::image::colors::black())),
	master_and_slaves_(
		rucksack::padding(
			sge::parse::json::find_and_convert_member<rucksack::scalar>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("visualization/border-size"))))),
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
			sge::renderer::resource_flags::none))
{
	monitor_parent_.viewport_widget().child(
		master_and_slaves_);

	master_and_slaves_.master_pane(
		velocity_arrows_.widget());

	for(
		flakelib::additional_planar_data::const_iterator it =
			simulation_.additional_planar_data().begin();
		it !=simulation_.additional_planar_data().end();
		++it)
	{
		fcppt::container::ptr::insert_unique_ptr_map(
			additional_data_,
			it->first,
			fcppt::make_unique_ptr<monitor::texture>(
				fcppt::ref(
					monitor_parent_),
				monitor::name(
					it->first),
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
					sge::parse::json::find_and_convert_member<monitor::rect::value_type>(
						_config_file,
						sge::parse::json::string_to_path(
							FCPPT_TEXT("visualization/")+it->first+FCPPT_TEXT("-scale"))))));

		master_and_slaves_.push_back_child(
			additional_data_.find(it->first)->second->widget());
	}
}

void
flakelib::visualization::arrow::update(
	flakelib::duration const &)
{
	velocity_arrows_.from_planar_object(
		simulation_.velocity());

	for(
		flakelib::additional_planar_data::const_iterator it =
			simulation_.additional_planar_data().begin();
		it !=simulation_.additional_planar_data().end();
		++it)
		for(additional_data_monitors::iterator it2 =
			additional_data_.begin();
			it2 != additional_data_.end();
			++it2)
			if(it->first == it2->first)
				it2->second->from_planar_object(
					it->second);

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
