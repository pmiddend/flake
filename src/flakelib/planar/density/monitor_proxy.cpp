#include <flakelib/planar/density/monitor_proxy.hpp>
#include <flakelib/planar/monitor/planar_converter.hpp>
#include <fcppt/text.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/structure_cast.hpp>


flakelib::planar::density::monitor_proxy::monitor_proxy(
	monitor::parent &_parent,
	monitor::grid_dimensions const &_grid_dimensions,
	monitor::dim const &_dim,
	monitor::planar_converter &_planar_converter)
:
	density_texture_(
		_parent,
		monitor::name(
			FCPPT_TEXT("density")),
		_grid_dimensions,
		_dim,
		monitor::scaling_factor(
			1.0f)),
	planar_converter_(
		_planar_converter)
{
}

void
flakelib::planar::density::monitor_proxy::update(
	flakelib::buffer::planar_view<cl_float> const &_image)
{
	planar_converter_.scalar_to_texture(
		_image,
		density_texture_.cl_texture(),
		monitor::scaling_factor(
			density_texture_.scaling_factor()));
}

flakelib::planar::density::cursor_rectangle const
flakelib::planar::density::monitor_proxy::rectangle() const
{
	return
		density::cursor_rectangle(
			fcppt::math::vector::structure_cast<density::cursor_rectangle::vector>(
				density_texture_.content_widget().position()),
			fcppt::math::dim::structure_cast<density::cursor_rectangle::dim>(
				density_texture_.content_widget().size()));
}

flakelib::planar::monitor::texture &
flakelib::planar::density::monitor_proxy::monitor()
{
	return density_texture_;
}

flakelib::planar::density::monitor_proxy::~monitor_proxy()
{
}
