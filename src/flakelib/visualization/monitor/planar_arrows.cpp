#include <flakelib/visualization/monitor/parent.hpp>
#include <flakelib/visualization/monitor/planar_arrows.hpp>
#include <flakelib/visualization/monitor/dummy_sprite/parameters.hpp>
#include <rucksack/axis_policy2.hpp>
#include <sge/font/text/draw.hpp>
#include <sge/font/text/flags_none.hpp>
#include <sge/font/text/from_fcppt_string.hpp>
#include <sge/font/text/part.hpp>
#include <sge/font/text/size.hpp>
#include <sge/opencl/memory_object/renderer_buffer_lock_mode.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/first_index.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/viewport_size.hpp>
#include <sge/renderer/projection/dim.hpp>
#include <sge/renderer/projection/far.hpp>
#include <sge/renderer/projection/near.hpp>
#include <sge/renderer/projection/orthogonal_wh.hpp>
#include <sge/renderer/vf/dynamic/part_index.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/object.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/sprite/parameters.hpp>
#include <sge/texture/part_raw.hpp>
#include <fcppt/make_shared_ptr.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/text.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/vector/structure_cast.hpp>
#include <fcppt/config/external_begin.hpp>
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
				rucksack::preferred_size(),
				rucksack::is_expanding(
					false)),
			rucksack::axis_policy(
				rucksack::minimum_size(
					static_cast<rucksack::scalar>(
						size.h())),
				rucksack::preferred_size(),
				rucksack::is_expanding(
					false)),
			rucksack::aspect(
				1,
				1));
}
}

FCPPT_PP_PUSH_WARNING
FCPPT_PP_DISABLE_VC_WARNING(4355)
flakelib::visualization::monitor::planar_arrows::planar_arrows(
	monitor::parent &_parent,
	monitor::name const &_name,
	monitor::grid_dimensions const &_dimensions,
	monitor::arrow_scale const &_arrow_scale,
	monitor::grid_scale const &_grid_scale,
	sge::renderer::texture::planar_ptr const _optional_texture)
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
			child::parent().vertex_declaration(),
			sge::renderer::vf::dynamic::part_index(
				0u),
			sge::renderer::vertex_count(
				static_cast<sge::renderer::size_type>(
					dimensions_.content() * 2)),
			sge::renderer::resource_flags_field(
				sge::renderer::resource_flags::readable))),
	cl_vb_(
		child::parent().context(),
		*vb_,
		sge::opencl::memory_object::renderer_buffer_lock_mode::write_only),
	sprite_(),
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
						grid_scale_.get() *
						static_cast<monitor::scalar>(
							dimensions_.w()))),
				rucksack::preferred_size(),
				rucksack::is_expanding(
					false)),
			rucksack::axis_policy(
				rucksack::minimum_size(
					static_cast<rucksack::scalar>(
						grid_scale_.get() *
						static_cast<monitor::scalar>(
							dimensions_.h()))),
				rucksack::preferred_size(),
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

	if(_optional_texture)
	{
		sprite_.take(
			fcppt::make_unique_ptr<dummy_sprite::object>(
				dummy_sprite::parameters()
					.texture(
						fcppt::make_shared_ptr<sge::texture::part_raw>(
							_optional_texture))
					.system(
						child::parent().sprite_system())
					.elements()));
	}
}
FCPPT_PP_POP_WARNING

void
flakelib::visualization::monitor::planar_arrows::from_planar_object(
	flakelib::planar_object const &_planar_object)
{
	child::parent().to_vb(
		_planar_object,
		cl_vb_,
		grid_scale_,
		arrow_scale_);
}

fcppt::string const
flakelib::visualization::monitor::planar_arrows::name() const
{
	return name_;
}

void
flakelib::visualization::monitor::planar_arrows::render(
	monitor::optional_projection const &_projection)
{
	// Activate the shader and the vertex declaration
	sge::shader::scoped scoped_shader(
		child::parent().arrow_shader(),
		sge::shader::activate_everything());

	sge::renderer::scoped_vertex_buffer scoped_vb(
		child::parent().renderer(),
		*vb_);

	child::parent().arrow_shader().update_uniform(
		"projection",
		sge::shader::matrix(
			_projection.has_value()
			?
				*_projection
			:
				sge::renderer::projection::orthogonal_wh(
					fcppt::math::dim::structure_cast<sge::renderer::projection::dim>(
							sge::renderer::viewport_size(
								child::parent().renderer())),
					sge::renderer::projection::near(0.0f),
					sge::renderer::projection::far(10.0f)),
			sge::shader::matrix_flags::projection));

	child::parent().arrow_shader().update_uniform(
		"initial_position",
		fcppt::math::vector::structure_cast<sge::renderer::vector2>(
			sprite_box_.position()));

	child::parent().renderer().render_nonindexed(
		sge::renderer::first_vertex(
			0),
		sge::renderer::vertex_count(
			vb_->size()),
		sge::renderer::nonindexed_primitive_type::line);
}

void
flakelib::visualization::monitor::planar_arrows::update()
{
	if(sprite_)
	{
		sprite_->pos(
			fcppt::math::vector::structure_cast<monitor::dummy_sprite::object::vector>(
				sprite_box_.position()));

		sprite_->size(
			fcppt::math::dim::structure_cast<monitor::dummy_sprite::object::dim>(
				sprite_box_.size()));
	}
}

rucksack::widget::base &
flakelib::visualization::monitor::planar_arrows::widget()
{
	return box_parent_;
}

flakelib::visualization::monitor::planar_arrows::~planar_arrows()
{
}
