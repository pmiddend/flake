#include <flakelib/density/monitor_proxy.hpp>
#include <flakelib/monitor/planar_converter.hpp>
#include <fcppt/text.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/structure_cast.hpp>


flakelib::density::monitor_proxy::monitor_proxy(
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
flakelib::density::monitor_proxy::update(
	sge::opencl::memory_object::image::planar &_image)
{
	planar_converter_.to_texture(
		flakelib::planar_object(
			&_image),
		density_texture_.cl_texture(),
		monitor::scaling_factor(
			density_texture_.scaling_factor()));
}

flakelib::density::cursor_rectangle const
flakelib::density::monitor_proxy::rectangle() const
{
	return
		density::cursor_rectangle(
			fcppt::math::vector::structure_cast<density::cursor_rectangle::vector>(
				density_texture_.content_widget().position()),
			fcppt::math::dim::structure_cast<density::cursor_rectangle::dim>(
				density_texture_.content_widget().size()));
}

flakelib::monitor::texture &
flakelib::density::monitor_proxy::monitor()
{
	return density_texture_;
}

flakelib::density::monitor_proxy::~monitor_proxy()
{
}
