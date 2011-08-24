#include "simulation_impl.hpp"
#include "vector2.hpp"
#include "source.hpp"
#include "advect.hpp"
#include "diffusion_coefficient.hpp"
#include "apply_sources.hpp"
#include "diffuse.hpp"
#include "vector_field_to_lines.hpp"
#include "texture_from_scalar_grid.hpp"
#include "density_sprite/parameters.hpp"
#include "generate_excentric_vector_field.hpp"
#include "scalar.hpp"
#include <sge/systems/instance.hpp>
#include <sge/line_drawer/color_format.hpp>
#include <sge/sprite/parameters_impl.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/image/color/any/convert.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_code.hpp>
#include <sge/input/keyboard/action.hpp>
#include <sge/sprite/default_equal.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <sge/line_drawer/render_to_screen.hpp>
#include <sge/texture/part_raw.hpp>
#include <sge/texture/part_ptr.hpp>
#include <mizuiro/image/make_const_view.hpp>
#include <mizuiro/color/init/luminance.hpp>
#include <mizuiro/color/object.hpp>
#include <fcppt/text.hpp>
#include <fcppt/tr1/functional.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
// swap, fill
#include <algorithm>
#include <iostream>

flake::simulation_impl::simulation_impl(
	sge::parse::json::object const &_config_file,
	sge::systems::instance const &_systems)
:
	systems_(
		_systems),
	line_drawer_(
		systems_.renderer()),
	cell_size_(
		sge::parse::json::find_and_convert_member<flake::vector2>(
			_config_file,
			sge::parse::json::path(
				FCPPT_TEXT("cell-size")))),
	field_position_(
		sge::parse::json::find_and_convert_member<flake::vector2>(
			_config_file,
			sge::parse::json::path(
				FCPPT_TEXT("field-position")))),
	arrow_length_(
		sge::parse::json::find_and_convert_member<flake::scalar>(
			_config_file,
			sge::parse::json::path(
				FCPPT_TEXT("arrow-length")))),
	diffusion_coefficient_(
		flake::diffusion_coefficient(
			sge::parse::json::find_and_convert_member<flake::scalar>(
				_config_file,
				sge::parse::json::path(
					FCPPT_TEXT("diffusion-coefficient"))))),
	main_grid_(
		flake::generate_excentric_vector_field<flake::vector2_grid2>(
			sge::parse::json::find_and_convert_member<flake::vector2_grid2::dim>(
				_config_file,
				sge::parse::json::path(
					FCPPT_TEXT("field-dimensions"))),
			sge::parse::json::find_and_convert_member<flake::vector2>(
				_config_file,
				sge::parse::json::path(
					FCPPT_TEXT("reference-point"))),
			cell_size_)),
	density_grid_store_0_(
		main_grid_.size()),
	density_grid_store_1_(
		main_grid_.size()),
	density_grid_view_0_(
		&density_grid_store_0_),
	density_grid_view_1_(
		&density_grid_store_1_),
	density_sources_(),
	density_sprite_system_(
		systems_.renderer()),
	density_sprite_(
		density_sprite::parameters()
			.system(
				&density_sprite_system_)
			.pos(
				field_position_)
			.size(
				density_sprite::object::dim(
					static_cast<density_sprite::object::unit>(main_grid_.size().w()) * cell_size_.x(),
					static_cast<density_sprite::object::unit>(main_grid_.size().h()) * cell_size_.y()))
			.elements()),
	density_input_connection_(
		systems_.keyboard_collector().key_callback(
			sge::input::keyboard::action(
				sge::input::keyboard::key_code::space,
				std::tr1::bind(
					&simulation_impl::density_callback,
					this))))
{
	std::fill(
		density_grid_store_0_.begin(),
		density_grid_store_0_.end(),
		flake::scalar(
			0.0f));

	density_sources_.push_back(
		density_source(
			sge::parse::json::find_and_convert_member<density_source::position_type>(
				_config_file,
				sge::parse::json::path(
					FCPPT_TEXT("density-source-position"))),
			sge::parse::json::find_and_convert_member<density_source::value_type>(
				_config_file,
				sge::parse::json::path(
					FCPPT_TEXT("density-source-intensity")))));
}

void
flake::simulation_impl::update()
{
	flake::vector_field_to_lines(
		main_grid_,
		line_drawer_,
		field_position_,
		cell_size_,
		arrow_length_,
		sge::image::color::any::convert<sge::line_drawer::color_format>(
			sge::image::colors::red()));
}

void
flake::simulation_impl::render()
{
	density_sprite_.texture(
		sge::texture::part_ptr(
			fcppt::make_shared_ptr<sge::texture::part_raw>(
				flake::texture_from_scalar_grid(
					systems_.renderer(),
					*density_grid_view_0_))));

	density_sprite_system_.render_all(
		sge::sprite::default_equal());

	/*
	sge::line_drawer::render_to_screen(
		systems_.renderer(),
		line_drawer_);
	*/

}

flake::simulation_impl::~simulation_impl()
{
}

void
flake::simulation_impl::density_callback()
{
	static unsigned counter = 0;
	std::cout << counter++ << ": Updating\n";

	flake::scalar_duration const time_delta(
		0.5f);

	flake::apply_sources(
		*density_grid_view_0_,
		density_sources_,
		time_delta);

	flake::diffuse(
		boundary_type::density,
		*density_grid_view_0_,
		*density_grid_view_1_,
		diffusion_coefficient_,
		time_delta);

	std::swap(
		density_grid_view_0_,
		density_grid_view_1_);

	flake::advect(
		flake::boundary_type::density,
		*density_grid_view_0_,
		*density_grid_view_1_,
		main_grid_,
		time_delta);

	std::swap(
		density_grid_view_0_,
		density_grid_view_1_);
}
