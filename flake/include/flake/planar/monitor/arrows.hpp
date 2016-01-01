#ifndef FLAKE_PLANAR_MONITOR_ARROWS_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_ARROWS_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
#include <flake/planar/monitor/arrow_scale.hpp>
#include <flake/planar/monitor/child.hpp>
#include <flake/planar/monitor/grid_dimensions.hpp>
#include <flake/planar/monitor/grid_scale.hpp>
#include <flake/planar/monitor/name.hpp>
#include <flake/planar/monitor/optional_background_texture.hpp>
#include <flake/planar/monitor/parent_fwd.hpp>
#include <flake/planar/monitor/rect.hpp>
#include <flake/planar/monitor/dummy_sprite/object.hpp>
#include <sge/font/draw/static_text.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/renderer/vertex/buffer_unique_ptr.hpp>
#include <sge/rucksack/widget/box.hpp>
#include <sge/rucksack/widget/dummy.hpp>
#include <sge/sprite/object_decl.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/optional/object.hpp>
#include <fcppt/math/box/object_impl.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/vector/object_impl.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
class arrows
:
	public monitor::child
{
FCPPT_NONCOPYABLE(
	arrows);
public:
	FLAKE_DETAIL_SYMBOL
	arrows(
		monitor::parent &,
		monitor::name const &,
		monitor::grid_dimensions const &,
		monitor::arrow_scale const &,
		monitor::grid_scale const &,
		monitor::optional_background_texture const &);

	FLAKE_DETAIL_SYMBOL
	sge::opencl::memory_object::buffer &
	cl_buffer();

	FLAKE_DETAIL_SYMBOL
	monitor::arrow_scale const
	arrow_scale() const;

	FLAKE_DETAIL_SYMBOL
	monitor::grid_scale const
	grid_scale() const;

	fcppt::string const
	name() const;

	void
	render(
		sge::renderer::context::ffp &,
		monitor::optional_projection const &);

	void
	update();

	FLAKE_DETAIL_SYMBOL
	sge::rucksack::widget::base &
	widget();

	FLAKE_DETAIL_SYMBOL
	~arrows();
private:
	fcppt::string const name_;
	monitor::grid_dimensions::value_type const dimensions_;
	monitor::arrow_scale const arrow_scale_;
	monitor::grid_scale const grid_scale_;
	sge::renderer::vertex::buffer_unique_ptr const vb_;
	sge::opencl::memory_object::buffer cl_vb_;
	typedef
	fcppt::optional::object<
		dummy_sprite::object
	>
	optional_dummy_sprite;
	optional_dummy_sprite sprite_;
	sge::rucksack::widget::box box_parent_;
	sge::rucksack::widget::dummy font_box_;
	sge::rucksack::widget::dummy sprite_box_;
	sge::font::draw::static_text font_renderable_;

	void
	render_font(
		sge::renderer::context::ffp &,
		monitor::optional_projection const &);

	void
	render_arrows(
		sge::renderer::context::ffp &,
		monitor::optional_projection const &);
};
}
}
}

#endif
