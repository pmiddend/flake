#include <flake/planar/monitor/arrows.hpp>
#include <flake/planar/monitor/font_axis_policy.hpp>
#include <flake/planar/monitor/parent.hpp>
#include <sge/font/dim.hpp>
#include <sge/font/from_fcppt_string.hpp>
#include <sge/font/object.hpp>
#include <sge/font/system.hpp>
#include <sge/font/text.hpp>
#include <sge/font/text_parameters.hpp>
#include <sge/font/align_h/left.hpp>
#include <sge/font/align_h/variant.hpp>
#include <sge/opencl/memory_object/renderer_buffer_lock_mode.hpp>
#include <sge/renderer/primitive_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/vector2.hpp>
#include <sge/renderer/context/ffp.hpp>
#include <sge/renderer/device/ffp.hpp>
#include <sge/renderer/index/first.hpp>
#include <sge/renderer/projection/dim.hpp>
#include <sge/renderer/projection/far.hpp>
#include <sge/renderer/projection/near.hpp>
#include <sge/renderer/projection/orthogonal_wh.hpp>
#include <sge/renderer/state/ffp/transform/object.hpp>
#include <sge/renderer/state/ffp/transform/object_unique_ptr.hpp>
#include <sge/renderer/state/ffp/transform/parameters.hpp>
#include <sge/renderer/state/ffp/transform/scoped.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <sge/renderer/texture/emulate_srgb.hpp>
#include <sge/renderer/vertex/buffer.hpp>
#include <sge/renderer/vertex/buffer_parameters.hpp>
#include <sge/renderer/vertex/count.hpp>
#include <sge/renderer/vertex/scoped_buffer.hpp>
#include <sge/renderer/vertex/scoped_declaration.hpp>
#include <sge/renderer/vf/dynamic/part_index.hpp>
#include <sge/rucksack/axis.hpp>
#include <sge/rucksack/axis_policy2.hpp>
#include <sge/rucksack/padding.hpp>
#include <sge/shader/scoped_pair.hpp>
#include <sge/sprite/projection_matrix.hpp>
#include <sge/sprite/roles/connection.hpp>
#include <sge/sprite/roles/pos.hpp>
#include <sge/sprite/roles/size.hpp>
#include <sge/sprite/roles/texture0.hpp>
#include <sge/texture/part_raw_ref.hpp>
#include <fcppt/from_optional.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/maybe_void.hpp>
#include <fcppt/text.hpp>
#include <fcppt/cast/int_to_float_fun.hpp>
#include <fcppt/cast/size_fun.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/box/object_impl.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/contents.hpp>
#include <fcppt/math/dim/null.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/matrix/identity.hpp>
#include <fcppt/math/vector/null.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/preprocessor/disable_vc_warning.hpp>
#include <fcppt/preprocessor/pop_warning.hpp>
#include <fcppt/preprocessor/push_warning.hpp>
#include <fcppt/config/external_begin.hpp>
#include <limits>
#include <fcppt/config/external_end.hpp>


FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_VC_WARNING(4355)
flake::planar::monitor::arrows::arrows(
	monitor::parent &_parent,
	monitor::name const &_name,
	monitor::grid_dimensions const &_dimensions,
	monitor::arrow_scale const &_arrow_scale,
	monitor::grid_scale const &_grid_scale,
	monitor::optional_background_texture const &_optional_texture)
:
	monitor::child(
		_parent),
	name_(
		_name.get()),
	dimensions_(
		_dimensions.get()),
	arrow_scale_(
		_arrow_scale),
	grid_scale_(
		_grid_scale),
	vb_(
		child::parent().renderer().create_vertex_buffer(
			sge::renderer::vertex::buffer_parameters(
				child::parent().arrow_vertex_declaration(),
				sge::renderer::vf::dynamic::part_index(
					0u),
				sge::renderer::vertex::count(
					static_cast<sge::renderer::size_type>(
						fcppt::math::dim::contents(
							dimensions_
						)
						* 6
					)
				),
				sge::renderer::resource_flags_field{
					sge::renderer::resource_flags::readable}))),
	cl_vb_(
		child::parent().cl_context(),
		*vb_,
		sge::opencl::memory_object::renderer_buffer_lock_mode::write_only),
	sprite_(),
	box_parent_(
		sge::rucksack::axis::y,
		sge::rucksack::padding{0}),
	font_box_(
		flake::planar::monitor::font_axis_policy(
			child::parent().font(),
			sge::font::text_parameters(
				sge::font::align_h::variant(
					sge::font::align_h::left())),
			sge::font::from_fcppt_string(
				this->name()))),
	sprite_box_(
		sge::rucksack::axis_policy2(
			sge::rucksack::axis_policy(
				sge::rucksack::preferred_size(
					static_cast<sge::rucksack::scalar>(
						grid_scale_.get() *
						static_cast<monitor::scalar>(
							dimensions_.w())))),
			sge::rucksack::axis_policy(
				sge::rucksack::preferred_size(
					static_cast<sge::rucksack::scalar>(
						grid_scale_.get() *
						static_cast<monitor::scalar>(
							dimensions_.h())))))),
	font_renderable_(
		child::parent().renderer(),
		child::parent().font(),
		sge::font::from_fcppt_string(
			this->name()),
		sge::font::text_parameters(
			sge::font::align_h::variant(
				sge::font::align_h::left())),
		fcppt::math::vector::null<
			sge::font::vector
		>(),
		child::parent().font_color().get(),
		sge::renderer::texture::emulate_srgb::no)
{
	box_parent_.push_back_child(
		font_box_,
		sge::rucksack::alignment::left_or_top);

	box_parent_.push_back_child(
		sprite_box_,
		sge::rucksack::alignment::left_or_top);

	fcppt::maybe_void(
		_optional_texture,
		[
			this
		](
			sge::renderer::texture::planar &_texture
		)
		{
			sprite_ =
				optional_dummy_sprite(
					dummy_sprite::object(
						sge::sprite::roles::texture0{} =
							dummy_sprite::object::texture_type{
								fcppt::make_shared_ptr<
									sge::texture::part_raw_ref
								>(
									_texture
								)
							},
						sge::sprite::roles::connection{} =
							this->parent().sprite_collection().connection(
								0
							),
						sge::sprite::roles::pos{} =
							fcppt::math::vector::null<
								dummy_sprite::object::vector
							>(),
						sge::sprite::roles::size{} =
							fcppt::math::dim::null<
								dummy_sprite::object::dim
							>()
					)
				);
		}
	);
}
FCPPT_PP_POP_WARNING

sge::opencl::memory_object::buffer &
flake::planar::monitor::arrows::cl_buffer()
{
	return cl_vb_;
}

flake::planar::monitor::arrow_scale const
flake::planar::monitor::arrows::arrow_scale() const
{
	return arrow_scale_;
}

flake::planar::monitor::grid_scale const
flake::planar::monitor::arrows::grid_scale() const
{
	return grid_scale_;
}

fcppt::string const
flake::planar::monitor::arrows::name() const
{
	return name_;
}

void
flake::planar::monitor::arrows::render(
	sge::renderer::context::ffp &_context,
	monitor::optional_projection const &_projection)
{
	this->render_font(
		_context,
		_projection);

	this->render_arrows(
		_context,
		_projection);
}

void
flake::planar::monitor::arrows::update()
{
	fcppt::maybe_void(
		sprite_,
		[
			this
		](
			dummy_sprite::object &_sprite
		)
		{
			_sprite.pos(
				fcppt::math::vector::structure_cast<
					monitor::dummy_sprite::object::vector,
					fcppt::cast::size_fun>(
					sprite_box_.position()));

			_sprite.size(
				fcppt::math::dim::structure_cast<
					monitor::dummy_sprite::object::dim,
					fcppt::cast::size_fun>(
					sprite_box_.size()));
		}
	);
}

sge::rucksack::widget::base &
flake::planar::monitor::arrows::widget()
{
	return box_parent_;
}

flake::planar::monitor::arrows::~arrows()
{
}

void
flake::planar::monitor::arrows::render_font(
	sge::renderer::context::ffp &_context,
	monitor::optional_projection const &_projection)
{
	sge::renderer::state::ffp::transform::object_unique_ptr const world_state(
		child::parent().renderer().create_transform_state(
			sge::renderer::state::ffp::transform::parameters(
				fcppt::math::matrix::identity<
					sge::renderer::matrix4
				>()
			)
		)
	);

	sge::renderer::state::ffp::transform::scoped const world_transform(
		_context,
		sge::renderer::state::ffp::transform::mode::world,
		*world_state);

	sge::renderer::state::ffp::transform::object_unique_ptr const projection_state(
		child::parent().renderer().create_transform_state(
			sge::renderer::state::ffp::transform::parameters(
				fcppt::from_optional(
					_projection,
					[
						this
					]{
						return
							// FIXME: This is broken
							sge::sprite::projection_matrix(
								this->parent().renderer().onscreen_target().viewport()
							).get_unsafe();
					}
				)
			)
		)
	);

	sge::renderer::state::ffp::transform::scoped const projection_transform(
		_context,
		sge::renderer::state::ffp::transform::mode::projection,
		*projection_state);

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
flake::planar::monitor::arrows::render_arrows(
	sge::renderer::context::ffp &_context,
	monitor::optional_projection const &_projection)
{
	sge::shader::scoped_pair scoped_shader(
		_context,
		child::parent().arrow_shader());

	sge::renderer::vertex::scoped_declaration scoped_vertex_declaration(
		_context,
		child::parent().arrow_vertex_declaration());

	sge::renderer::vertex::scoped_buffer scoped_vb(
		_context,
		*vb_);

	child::parent().arrow_projection(
		fcppt::from_optional(
			_projection,
			[
				this
			]{
				return
					sge::renderer::projection::orthogonal_wh(
						fcppt::math::dim::structure_cast<
							sge::renderer::projection::dim,
							fcppt::cast::int_to_float_fun>(
							this->parent().renderer().onscreen_target().viewport().get().size()),
						sge::renderer::projection::near(0.0f),
						sge::renderer::projection::far(10.0f));
			}
		)
	);


	child::parent().arrow_position(
		fcppt::math::vector::structure_cast<
			sge::renderer::vector2,
			fcppt::cast::int_to_float_fun>(
			sprite_box_.position()));

	_context.render_nonindexed(
		sge::renderer::vertex::first(
			0u),
		sge::renderer::vertex::count(
			vb_->linear_size()),
		sge::renderer::primitive_type::line_list);
}
