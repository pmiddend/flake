#include <flakelib/visualization/arrow.hpp>
#include <flakelib/visualization/dummy_sprite/parameters.hpp>
#include <flakelib/simulation/base.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/image/colors.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/stage.hpp>
#include <sge/renderer/state/bool.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/trampoline.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/address_mode.hpp>
#include <sge/renderer/texture/create_planar_from_view.hpp>
#include <sge/renderer/texture/filter/point.hpp>
#include <sge/renderer/texture/filter/scoped.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/sprite/render_one.hpp>
#include <sge/texture/part_raw.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/basic_impl.hpp>
#include <fcppt/text.hpp>

flakelib::visualization::arrow::arrow(
	sge::renderer::device &_renderer,
	sge::opencl::context::object &_context,
	sge::opencl::command_queue::object &_command_queue,
	simulation::base &_simulation,
	flakelib::boundary_view const &_boundary,
	sge::parse::json::object const &_config_file)
:
	renderer_(
		_renderer),
	monitor_parent_(
		_renderer,
		_context,
		_command_queue),
	velocity_arrows_(
		monitor_parent_,
		monitor::grid_dimensions(
			fcppt::math::dim::structure_cast<monitor::grid_dimensions>(
				sge::image2d::view::size(
					_boundary.get()))),
		monitor::arrow_scale(
			sge::parse::json::find_and_convert_member<dummy_sprite::object::unit>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("visualization/arrow-scale")))),
		monitor::grid_scale(
			sge::parse::json::find_and_convert_member<dummy_sprite::object::unit>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("grid-scale")))),
		monitor::position(
			sge::parse::json::find_and_convert_member<monitor::position::value_type>(
				_config_file,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("visualization/grid-position"))))),
	simulation_(
		_simulation),
	sprite_system_(
		renderer_),
	sprite_object_(
		dummy_sprite::parameters()
			.pos(
				sge::parse::json::find_and_convert_member<sge::renderer::vector2>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("visualization/grid-position"))))
			.size(
				sge::parse::json::find_and_convert_member<dummy_sprite::object::unit>(
					_config_file,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("grid-scale"))) *
				fcppt::math::dim::structure_cast<dummy_sprite::object::dim>(
					sge::image2d::view::size(
						_boundary.get())))
			.texture(
				fcppt::make_shared_ptr<sge::texture::part_raw>(
					sge::renderer::texture::create_planar_from_view(
						renderer_,
						_boundary.get(),
						sge::renderer::texture::mipmap::off(),
						sge::renderer::texture::address_mode2(
							sge::renderer::texture::address_mode::clamp),
						sge::renderer::resource_flags::none)))
			.elements())
{
}

void
flakelib::visualization::arrow::update(
	flakelib::duration const &)
{
	velocity_arrows_.from_buffer(
		simulation_.vector_field());
}

void
flakelib::visualization::arrow::render()
{
	{
	sge::renderer::texture::filter::scoped scoped_texture_filter(
		renderer_,
		sge::renderer::stage(0),
		sge::renderer::texture::filter::point());

	sge::sprite::render_one(
		sprite_system_,
		sprite_object_);
	}

	velocity_arrows_.render();
}

sge::renderer::state::list const
flakelib::visualization::arrow::render_states() const
{
	return
		sge::renderer::state::list
			(sge::renderer::state::bool_::clear_back_buffer = true)
			(sge::renderer::state::color::back_buffer_clear_color = sge::image::colors::white());
}

flakelib::visualization::arrow::~arrow()
{
}
