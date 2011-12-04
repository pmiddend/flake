#include <flakelib/density/monitor_proxy.hpp>
#include <fcppt/text.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/math/dim/structure_cast.hpp>

flakelib::density::monitor_proxy::monitor_proxy(
	visualization::monitor::parent &_parent,
	visualization::monitor::grid_dimensions const &_grid_dimensions,
	visualization::monitor::dim const &_dim)
:
	density_texture_(
		_parent,
		visualization::monitor::name(
			FCPPT_TEXT("density")),
		_grid_dimensions,
		_dim,
		visualization::monitor::scaling_factor(
			1.0f))
{
}

void
flakelib::density::monitor_proxy::update(
	sge::opencl::memory_object::image::planar &_image)
{
	density_texture_.from_planar_object(
		flakelib::planar_object(
			&_image));
}

flakelib::density::cursor_rectangle const
flakelib::density::monitor_proxy::rectangle() const
{
	return
		density::cursor_rectangle(
			fcppt::math::vector::structure_cast<density::cursor_rectangle::vector>(
				density_texture_.widget().position()),
			fcppt::math::dim::structure_cast<density::cursor_rectangle::dim>(
				density_texture_.widget().size()));
}

flakelib::density::monitor_proxy::~monitor_proxy()
{
}
