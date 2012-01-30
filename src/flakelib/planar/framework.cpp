#include <flakelib/media_path_from_string.hpp>
#include <flakelib/planar/framework.hpp>
#include <flakelib/planar/laplace_solver/dynamic_factory.hpp>
#include <flakelib/planar/monitor/texture.hpp>
#include <flakelib/planar/simulation/stam/object.hpp>
#include <flakelib/planar/simulation/stam/wind_source.hpp>
#include <flakelib/planar/density/advector.hpp>
#include <flakelib/planar/density/cursor_splatter.hpp>
#include <flakelib/planar/density/monitor_proxy.hpp>
#include <flakelib/planar/monitor/parent.hpp>
#include <flakelib/planar/monitor/planar_arrows.hpp>
#include <flakelib/planar/monitor/planar_converter.hpp>
#include <flakelib/planar/buoyancy/object.hpp>
#include <rucksack/widget/master_and_slaves.hpp>
#include <rucksack/widget/viewport_adaptor.hpp>
#include <rucksack/widget/box/base.hpp>
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

flakelib::planar::framework::framework(
	sge::viewport::manager &_viewport_manager,
	sge::renderer::device &_renderer,
	sge::opencl::command_queue::object &_command_queue,
	sge::font::system &_font_system,
	flakelib::build_options const &_build_options,
	flakelib::planar::boundary_view const &_boundary,
	sge::parse::json::object const &_config_file,
	sge::input::cursor::object &_cursor,
	buffer_pool::object &_buffer_pool,
	utility::object &_utility)
:
	renderer_(
		_renderer),
	solver_factory_(
		fcppt::make_unique_ptr<laplace_solver::dynamic_factory>(
			fcppt::ref(
				_buffer_pool),
			fcppt::ref(
				_command_queue),
			sge::parse::json::find_and_convert_member<sge::parse::json::object>(
				_config_file,
				sge::parse::json::path(FCPPT_TEXT("simulation")) / FCPPT_TEXT("solver")),
			_build_options,
			fcppt::ref(
				_utility))),
	simulation_(
		fcppt::make_unique_ptr<simulation::stam::object>(
			fcppt::ref(
				_command_queue),
			_boundary,
			_build_options,
			fcppt::ref(
				_buffer_pool),
			fcppt::ref(
				_utility),
			fcppt::ref(
				solver_factory_->value()),
			flakelib::planar::simulation::stam::profiling_enabled(
				sge::parse::json::find_and_convert_member<bool>(
					_config_file,
					sge::parse::json::path(FCPPT_TEXT("simulation")) / FCPPT_TEXT("profiling-enabled"))),
			flakelib::planar::simulation::stam::use_maccormack(
				sge::parse::json::find_and_convert_member<bool>(
					_config_file,
					sge::parse::json::path(FCPPT_TEXT("simulation")) / FCPPT_TEXT("use-maccormack"))))),
	wind_source_(
		fcppt::make_unique_ptr<simulation::stam::wind_source>(
			fcppt::ref(
				_command_queue),
			_build_options,
			flakelib::planar::simulation::stam::external_force_magnitude(
				sge::parse::json::find_and_convert_member<cl_float>(
					_config_file,
					sge::parse::json::path(FCPPT_TEXT("simulation")) / FCPPT_TEXT("wind-speed"))))),
	monitor_parent_(
		fcppt::make_unique_ptr<monitor::parent>(
			fcppt::ref(
				_renderer),
			fcppt::ref(
				_command_queue),
			_font_system.create_font(
				flakelib::media_path_from_string(
					FCPPT_TEXT("fonts/main.ttf")),
				sge::parse::json::find_and_convert_member<sge::font::size_type>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("visualization/monitor-font-size")))),
			monitor::font_color(
				sge::image::colors::black()))),
	planar_converter_(
		fcppt::make_unique_ptr<monitor::planar_converter>(
			fcppt::ref(
				_command_queue),
			_build_options)),
	viewport_widget_(
		fcppt::make_unique_ptr<rucksack::widget::viewport_adaptor>(
			fcppt::ref(
				_viewport_manager),
			fcppt::ref(
				_renderer))),
	master_and_slave_(
		fcppt::make_unique_ptr<rucksack::widget::master_and_slaves>(
			rucksack::padding(
				sge::parse::json::find_and_convert_member<rucksack::scalar>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("visualization/master-and-slave-padding")))))),
	master_box_(
		fcppt::make_unique_ptr<rucksack::widget::box::base>(
			rucksack::axis::y,
			rucksack::aspect(
				1,
				1))),
	velocity_arrows_(
		fcppt::make_unique_ptr<monitor::planar_arrows>(
			fcppt::ref(
				*monitor_parent_),
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
						FCPPT_TEXT("visualization/monitor-arrow-scale")))),
			monitor::grid_scale(
				sge::parse::json::find_and_convert_member<monitor::arrow_scale::value_type>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("visualization/monitor-arrow-grid-scale")))),
			sge::renderer::texture::create_planar_from_view(
				renderer_,
				_boundary.get(),
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field(
					sge::renderer::resource_flags::none)))),
	density_advector_(
		fcppt::make_unique_ptr<density::advector>(
			fcppt::ref(
				_command_queue),
			fcppt::ref(
				_buffer_pool),
			fcppt::ref(
				_utility),
			_build_options,
			density::grid_dimensions(
				fcppt::math::dim::structure_cast<density::grid_dimensions::value_type>(
					sge::image2d::view::size(
						_boundary.get()))))),
	density_cursor_splatter_(
		fcppt::make_unique_ptr<density::cursor_splatter>(
			fcppt::ref(
				_command_queue),
			density::source_image(
				density_advector_->source_image()),
			fcppt::ref(
				_cursor),
			fcppt::ref(
				renderer_),
			_build_options,
			density::splat_radius(
				sge::parse::json::find_and_convert_member<density::splat_radius::value_type>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("density-splat-radius")))))),
	density_monitor_(
		fcppt::make_unique_ptr<density::monitor_proxy>(
			fcppt::ref(
				*monitor_parent_),
			monitor::grid_dimensions(
				fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
					sge::image2d::view::size(
						_boundary.get()))),
			monitor::rect::dim(
				static_cast<monitor::scalar>(
					velocity_arrows_->widget().axis_policy().x().minimum_size()),
				static_cast<monitor::scalar>(
					velocity_arrows_->widget().axis_policy().y().minimum_size())),
			fcppt::ref(
				*planar_converter_),
			monitor::name(
				FCPPT_TEXT("density")))),
	temperature_advector_(
		fcppt::make_unique_ptr<density::advector>(
			fcppt::ref(
				_command_queue),
			fcppt::ref(
				_buffer_pool),
			fcppt::ref(
				_utility),
			_build_options,
			density::grid_dimensions(
				fcppt::math::dim::structure_cast<density::grid_dimensions::value_type>(
					sge::image2d::view::size(
						_boundary.get()))))),
	temperature_cursor_splatter_(
		fcppt::make_unique_ptr<density::cursor_splatter>(
			fcppt::ref(
				_command_queue),
			density::source_image(
				temperature_advector_->source_image()),
			fcppt::ref(
				_cursor),
			fcppt::ref(
				renderer_),
			_build_options,
			density::splat_radius(
				sge::parse::json::find_and_convert_member<density::splat_radius::value_type>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("temperature-splat-radius")))))),
	temperature_monitor_(
		fcppt::make_unique_ptr<density::monitor_proxy>(
			fcppt::ref(
				*monitor_parent_),
			monitor::grid_dimensions(
				fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
					sge::image2d::view::size(
						_boundary.get()))),
			monitor::rect::dim(
				static_cast<monitor::scalar>(
					velocity_arrows_->widget().axis_policy().x().minimum_size()),
				static_cast<monitor::scalar>(
					velocity_arrows_->widget().axis_policy().y().minimum_size())),
			fcppt::ref(
				*planar_converter_),
			monitor::name(
				FCPPT_TEXT("temperature")))),
	buoyancy_(
		fcppt::make_unique_ptr<buoyancy::object>(
			fcppt::ref(
				_command_queue),
			_build_options,
			/*
			buoyancy::boundary(
				_boundary.get()),*/
			buoyancy::temperature_strength(
				sge::parse::json::find_and_convert_member<cl_float>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("temperature-strength")))),
			buoyancy::density_strength(
				sge::parse::json::find_and_convert_member<cl_float>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("density-strength")))),
			buoyancy::ambient_temperature(
				sge::parse::json::find_and_convert_member<cl_float>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("ambient-temperature")))))),
	additional_data_()
{
	viewport_widget_->child(
		*master_and_slave_);

	master_and_slave_->master_pane(
		*master_box_);

	master_box_->push_back_child(
		velocity_arrows_->widget(),
		rucksack::alignment::center);

	master_box_->push_back_child(
		density_monitor_->monitor().widget(),
		rucksack::alignment::center);

	master_and_slave_->push_back_child(
		temperature_monitor_->monitor().widget());

	rucksack::scalar const child_size_denominator =
		sge::parse::json::find_and_convert_member<rucksack::scalar>(
			_config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("visualization/child-size-denominator")));
	for(
		flakelib::planar::additional_scalar_data::const_iterator it =
			simulation_->additional_scalar_data().begin();
		it != simulation_->additional_scalar_data().end();
		++it)
	{
		fcppt::container::ptr::insert_unique_ptr_map(
			additional_data_,
			it->key(),
			fcppt::make_unique_ptr<monitor::texture>(
				fcppt::ref(
					*monitor_parent_),
				monitor::name(
					it->key()),
				monitor::grid_dimensions(
					fcppt::math::dim::structure_cast<monitor::grid_dimensions::value_type>(
						sge::image2d::view::size(
							_boundary.get()))),
				monitor::rect::dim(
					static_cast<monitor::scalar>(
						velocity_arrows_->widget().axis_policy().x().minimum_size()/child_size_denominator),
					static_cast<monitor::scalar>(
						velocity_arrows_->widget().axis_policy().y().minimum_size()/child_size_denominator)),
				monitor::scaling_factor(
					sge::parse::json::find_and_convert_member<monitor::scalar>(
						_config_file,
						sge::parse::json::path(FCPPT_TEXT("visualization"))
							/ FCPPT_TEXT("scales")
							/ it->key()))));

		master_and_slave_->push_back_child(
			additional_data_.find(it->key())->second->widget());
	}
}

void
flakelib::planar::framework::update(
	flakelib::duration const &_dt)
{
	wind_source_->update(
		simulation_->velocity());

	buoyancy_->update(
		buoyancy::velocity(
			simulation_->velocity()),
		buoyancy::temperature(
			temperature_advector_->density_image()),
		buoyancy::density(
			density_advector_->density_image()),
		_dt);

	simulation_->update(
		_dt);

	density_cursor_splatter_->update_cursor_rectangle(
		density_monitor_->rectangle());

	density_advector_->update(
		density::velocity_image(
			simulation_->velocity()),
		_dt);

	density_monitor_->update(
		density_advector_->density_image());

	temperature_cursor_splatter_->update_cursor_rectangle(
		temperature_monitor_->rectangle());

	temperature_advector_->update(
		density::velocity_image(
			simulation_->velocity()),
		_dt);

	temperature_monitor_->update(
		temperature_advector_->density_image());

	planar_converter_->to_arrow_vb(
		simulation_->velocity(),
		velocity_arrows_->cl_buffer(),
		monitor::grid_scale(
			velocity_arrows_->grid_scale()),
		monitor::arrow_scale(
			velocity_arrows_->arrow_scale()));

	for(
		flakelib::planar::additional_scalar_data::const_iterator it =
			simulation_->additional_scalar_data().begin();
		it != simulation_->additional_scalar_data().end();
		++it)
	{
		for(additional_data_monitors::iterator it2 =
			additional_data_.begin();
			it2 != additional_data_.end();
			++it2)
		{
			if(it->key() != it2->first)
				continue;

			planar_converter_->scalar_to_texture(
				*it->value(),
				it2->second->cl_texture(),
				monitor::scaling_factor(
					it2->second->scaling_factor()));
		}
	}

	monitor_parent_->update();
}

void
flakelib::planar::framework::render()
{
	monitor_parent_->render(
		monitor::optional_projection());
}

sge::renderer::state::list const
flakelib::planar::framework::render_states() const
{
	return
		sge::renderer::state::list
			(sge::renderer::state::bool_::clear_back_buffer = true)
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::grey());
}

flakelib::planar::framework::~framework()
{
}
