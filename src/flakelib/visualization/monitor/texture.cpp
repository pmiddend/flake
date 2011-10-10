#include <flakelib/visualization/monitor/texture.hpp>
#include <flakelib/visualization/monitor/dummy_sprite/parameters.hpp>
#include <flakelib/visualization/monitor/parent.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/renderer/device.hpp>
#include <sge/opencl/memory_object/flags_field.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/texture/part_raw.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>

flakelib::visualization::monitor::texture::texture(
	monitor::parent &_parent,
	monitor::name const &_name,
	monitor::grid_dimensions const &_grid_dimensions,
	monitor::rect const &_rect)
:
	monitor::child(
		_parent),
	name_(
		_name.get()),
	renderer_texture_(
		child::parent().renderer().create_planar_texture(
			sge::renderer::texture::planar_parameters(
				fcppt::math::dim::structure_cast<sge::renderer::dim2>(
					_grid_dimensions.get()),
				sge::image::color::format::l8,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::texture::address_mode2(
					sge::renderer::texture::address_mode::clamp),
				sge::renderer::resource_flags::none,
				sge::renderer::texture::capabilities_field::null()))),
	cl_texture_(
		child::parent().context(),
		sge::opencl::memory_object::flags::write,
		*renderer_texture_)/*,
	sprite_(
		dummy_sprite::parameters()
			.pos(
				fcppt::math::vector::structure_cast<dummy_sprite::object::vector>(
					_rect.pos()))
			.size(
				fcppt::math::dim::structure_cast<dummy_sprite::object::dim>(
					_rect.size()))
			.texture(
				fcppt::make_shared_ptr<sge::texture::part_raw>(
					renderer_texture_))
			.elements())*/
{
}

void
flakelib::visualization::monitor::texture::from_planar_object(
	flakelib::planar_object const &_planar_object)
{
	child::parent().to_texture(
		_planar_object,
		cl_texture_,
		monitor::scaling_factor(
			1.0f));
}

void
flakelib::visualization::monitor::texture::position(
	monitor::rect::vector const &_position)
{
	/*
	sprite_.pos(
		fcppt::math::vector::structure_cast<dummy_sprite::object::vector>(
			_position));
			*/
}

void
flakelib::visualization::monitor::texture::render()
{
}

fcppt::string const
flakelib::visualization::monitor::texture::name() const
{
	return name_;
}

flakelib::visualization::monitor::rect const
flakelib::visualization::monitor::texture::area() const
{
	return monitor::rect(monitor::rect::vector::null(),monitor::rect::dim::null());
	/*
	return 
		monitor::rect(
			fcppt::math::vector::structure_cast<monitor::rect::vector>(
				sprite_.pos()),
			fcppt::math::dim::structure_cast<monitor::rect::dim>(
				sprite_.size()));
				*/
}

flakelib::visualization::monitor::texture::~texture()
{
}
