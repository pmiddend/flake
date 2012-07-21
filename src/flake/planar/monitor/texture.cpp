#include <flake/planar/monitor/parent.hpp>
#include <flake/planar/monitor/texture.hpp>
#include <flake/planar/monitor/dummy_sprite/parameters.hpp>
#include <rucksack/axis_policy2.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/from_fcppt_string.hpp>
#include <sge/font/text/part.hpp>
#include <sge/font/text/size.hpp>
#include <sge/opencl/memory_object/flags_field.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/scoped_transform.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/sprite/projection_matrix.hpp>
#include <sge/texture/part_raw.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/box/object_impl.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/output.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/config/external_begin.hpp>
#include <iostream>
#include <limits>
#include <fcppt/config/external_end.hpp>


namespace
{
rucksack::axis_policy2 const
font_axis_policy(
	sge::font::metrics &_font_metrics,
	sge::font::text::string const &_text)
{
	sge::font::dim const size =
		sge::font::text::size(
			_font_metrics,
			_text,
			sge::font::dim(
				std::numeric_limits<sge::font::dim::value_type>::max(),
				std::numeric_limits<sge::font::dim::value_type>::max()),
			sge::font::text::flags::none).size();

	return
		rucksack::axis_policy2(
			rucksack::axis_policy(
				rucksack::minimum_size(
					static_cast<rucksack::scalar>(
						size.w())),
				rucksack::preferred_size(
					rucksack::optional_scalar()),
				rucksack::is_expanding(
					false)),
			rucksack::axis_policy(
				rucksack::minimum_size(
					static_cast<rucksack::scalar>(
						size.h())),
				rucksack::preferred_size(
					rucksack::optional_scalar()),
				rucksack::is_expanding(
					false)),
			rucksack::aspect(
				1,
				1));
}
}

FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_VC_WARNING(4355)
flake::planar::monitor::texture::texture(
	monitor::parent &_parent,
	monitor::name const &_name,
	monitor::grid_dimensions const &_grid_dimensions,
	monitor::texture_size const &_texture_size,
	monitor::scaling_factor const &_scaling)
:
	monitor::child(
		_parent),
	name_(
		_name.get()),
	scaling_(
		_scaling.get()),
	renderer_texture_(
		child::parent().renderer().create_planar_texture(
			sge::renderer::texture::planar_parameters(
				fcppt::math::dim::structure_cast<sge::renderer::dim2>(
					_grid_dimensions.get()),
				sge::image::color::format::rgba32f,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field::null(),
				sge::renderer::texture::capabilities_field::null()))),
	cl_texture_(
		child::parent().cl_context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::write),
		*renderer_texture_),
	sprite_(
		dummy_sprite::parameters()
			.size(
				_texture_size.get())
			.texture(
				fcppt::make_shared_ptr<sge::texture::part_raw>(
					fcppt::ref(
						*renderer_texture_)))
			.connection(
				child::parent().sprite_collection().connection(
					0))),
	box_parent_(
		rucksack::axis::y,
		rucksack::aspect(
			1,
			1)),
	font_box_(
		::font_axis_policy(
			child::parent().font_metrics(),
			sge::font::text::from_fcppt_string(
				this->name()))),
	sprite_box_(
		rucksack::axis_policy2(
			rucksack::axis_policy(
				rucksack::minimum_size(
					static_cast<rucksack::scalar>(
						_texture_size.get().w())),
				rucksack::preferred_size(
					rucksack::optional_scalar()),
				rucksack::is_expanding(
					false)),
			rucksack::axis_policy(
				rucksack::minimum_size(
					static_cast<rucksack::scalar>(
						_texture_size.get().h())),
				rucksack::preferred_size(
					rucksack::optional_scalar()),
				rucksack::is_expanding(
					false)),
			rucksack::aspect(
				1,
				1)))
{
	box_parent_.push_back_child(
		font_box_,
		rucksack::alignment::left_or_top);

	box_parent_.push_back_child(
		sprite_box_,
		rucksack::alignment::left_or_top);
}
FCPPT_PP_POP_WARNING

sge::opencl::memory_object::image::planar &
flake::planar::monitor::texture::cl_texture()
{
	return cl_texture_;
}

flake::planar::monitor::scalar
flake::planar::monitor::texture::scaling_factor() const
{
	return scaling_;
}

void
flake::planar::monitor::texture::render(
	sge::renderer::context::object &_context,
	monitor::optional_projection const &_projection)
{
	sge::renderer::scoped_transform world_transform(
		_context,
		sge::renderer::matrix_mode::world,
		sge::renderer::matrix4::identity());

	sge::renderer::scoped_transform projection_transform(
		_context,
		sge::renderer::matrix_mode::projection,
		_projection
		?
			*_projection
		:
			sge::sprite::projection_matrix(
				child::parent().renderer().onscreen_target().viewport()));

	sge::font::text::draw(
		_context,
		child::parent().font_metrics(),
		child::parent().font_drawer(),
		sge::font::text::from_fcppt_string(
			this->name()),
		sge::font::rect(
			fcppt::math::vector::structure_cast<sge::font::rect::vector>(
				font_box_.position()),
			fcppt::math::dim::structure_cast<sge::font::rect::dim>(
				font_box_.size())),
		sge::font::text::align_h::center,
		sge::font::text::align_v::center,
		sge::font::text::flags::none);
}

void
flake::planar::monitor::texture::update()
{
	box_parent_.relayout();

	sprite_.pos(
		fcppt::math::vector::structure_cast<dummy_sprite::object::vector>(
			sprite_box_.position()));

	sprite_.size(
		fcppt::math::dim::structure_cast<dummy_sprite::object::dim>(
			sprite_box_.size()));
}

rucksack::widget::base &
flake::planar::monitor::texture::widget()
{
	return box_parent_;
}

rucksack::widget::base const &
flake::planar::monitor::texture::widget() const
{
	return box_parent_;
}

rucksack::widget::base &
flake::planar::monitor::texture::content_widget()
{
	return sprite_box_;
}

rucksack::widget::base const &
flake::planar::monitor::texture::content_widget() const
{
	return sprite_box_;
}

fcppt::string const
flake::planar::monitor::texture::name() const
{
	return name_;
}

flake::planar::monitor::texture::~texture()
{
}
