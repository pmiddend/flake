#include <flake/planar/monitor/font_axis_policy.hpp>
#include <flake/planar/monitor/parent.hpp>
#include <flake/planar/monitor/texture.hpp>
#include <flake/planar/monitor/dummy_sprite/parameters.hpp>
#include <sge/font/dim.hpp>
#include <sge/font/from_fcppt_string.hpp>
#include <sge/font/object.hpp>
#include <sge/font/text.hpp>
#include <sge/font/text_parameters.hpp>
#include <sge/font/align_h/left.hpp>
#include <sge/image/color/predef.hpp>
#include <sge/opencl/memory_object/flags_field.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/state/ffp/transform/object.hpp>
#include <sge/renderer/state/ffp/transform/object_unique_ptr.hpp>
#include <sge/renderer/state/ffp/transform/parameters.hpp>
#include <sge/renderer/state/ffp/transform/scoped.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/rucksack/axis.hpp>
#include <sge/rucksack/axis_policy2.hpp>
#include <sge/rucksack/padding.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/sprite/projection_matrix.hpp>
#include <sge/texture/part_raw_ref.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/cast/size_fun.hpp>
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


FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_VC_WARNING(4355)
flake::planar::monitor::texture::texture(
	flake::planar::monitor::parent &_parent,
	flake::planar::monitor::name const &_name,
	flake::planar::monitor::grid_dimensions const &_grid_dimensions,
	flake::planar::monitor::texture_size const &_texture_size,
	flake::planar::monitor::scaling_factor const &_scaling)
:
	flake::planar::monitor::child(
		_parent),
	name_(
		_name.get()),
	scaling_(
		_scaling.get()),
	renderer_texture_(
		child::parent().renderer().create_planar_texture(
			sge::renderer::texture::planar_parameters(
				_grid_dimensions.get(),
				sge::renderer::texture::color_format(
					sge::image::color::format::rgba32f,
					sge::renderer::texture::emulate_srgb::no),
				sge::renderer::texture::mipmap::off(),
				sge::renderer::resource_flags_field::null(),
				sge::renderer::texture::capabilities_field::null()))),
	cl_texture_(
		child::parent().cl_context(),
		sge::opencl::memory_object::flags_field{
			sge::opencl::memory_object::flags::write},
		*renderer_texture_),
	sprite_(
		flake::planar::monitor::dummy_sprite::parameters()
			.size(
				_texture_size.get())
			.texture(
				fcppt::make_shared_ptr<sge::texture::part_raw_ref>(
					*renderer_texture_))
			.connection(
				child::parent().sprite_collection().connection(
					0))),
	box_parent_(
		sge::rucksack::axis::y,
		sge::rucksack::padding{0}),
	font_box_(
		flake::planar::monitor::font_axis_policy(
			child::parent().font(),
			sge::font::text_parameters(
				sge::font::align_h::left()),
			sge::font::from_fcppt_string(
				this->name()))),
	sprite_box_(
		sge::rucksack::axis_policy2(
			sge::rucksack::axis_policy(
				sge::rucksack::preferred_size(
					static_cast<sge::rucksack::scalar>(
						_texture_size.get().w()))),
			sge::rucksack::axis_policy(
				sge::rucksack::preferred_size(
					static_cast<sge::rucksack::scalar>(
						_texture_size.get().h()))))),
	font_renderable_(
		child::parent().renderer(),
		child::parent().font(),
		sge::font::from_fcppt_string(
			this->name()),
		sge::font::text_parameters(
			sge::font::align_h::left()),
		sge::font::vector::null(),
		child::parent().font_color().get(),
		sge::renderer::texture::emulate_srgb::no)
{
	box_parent_.push_back_child(
		font_box_,
		sge::rucksack::alignment::left_or_top);

	box_parent_.push_back_child(
		sprite_box_,
		sge::rucksack::alignment::left_or_top);
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
	sge::renderer::context::ffp &_context,
	monitor::optional_projection const &_projection)
{
	sge::renderer::state::ffp::transform::object_unique_ptr const
		projection_state(
			child::parent().renderer().create_transform_state(
				sge::renderer::state::ffp::transform::parameters(
					_projection
					?
						*_projection
					:
						*sge::sprite::projection_matrix(
							child::parent().renderer().onscreen_target().viewport())))),
		world_state(
			child::parent().renderer().create_transform_state(
				sge::renderer::state::ffp::transform::parameters(
					sge::renderer::matrix4::identity())));

	sge::renderer::state::ffp::transform::scoped const
		projection_transform(
			_context,
			sge::renderer::state::ffp::transform::mode::projection,
			*projection_state),
		world_transform(
			_context,
			sge::renderer::state::ffp::transform::mode::world,
			*world_state);

	font_renderable_.pos(
		font_box_.position());

	font_renderable_.draw_advanced(
		_context,
		sge::font::draw::set_matrices(
			false),
		sge::font::draw::set_states(
			true));
}

void
flake::planar::monitor::texture::update()
{
	box_parent_.relayout();

	sprite_.pos(
		fcppt::math::vector::structure_cast<
			dummy_sprite::object::vector,
			fcppt::cast::size_fun>(
			sprite_box_.position()));

	sprite_.size(
		fcppt::math::dim::structure_cast<
			dummy_sprite::object::dim,
			fcppt::cast::size_fun>(
			sprite_box_.size()));
}

sge::rucksack::widget::base &
flake::planar::monitor::texture::widget()
{
	return box_parent_;
}

sge::rucksack::widget::base const &
flake::planar::monitor::texture::widget() const
{
	return box_parent_;
}

sge::rucksack::widget::base &
flake::planar::monitor::texture::content_widget()
{
	return sprite_box_;
}

sge::rucksack::widget::base const &
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
