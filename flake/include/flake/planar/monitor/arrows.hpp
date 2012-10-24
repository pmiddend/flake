#ifndef FLAKE_PLANAR_MONITOR_ARROWS_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_ARROWS_HPP_INCLUDED

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
#include <sge/renderer/vertex_buffer_scoped_ptr.hpp>
#include <sge/rucksack/widget/dummy.hpp>
#include <sge/rucksack/widget/box/base.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
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
	arrows(
		monitor::parent &,
		monitor::name const &,
		monitor::grid_dimensions const &,
		monitor::arrow_scale const &,
		monitor::grid_scale const &,
		monitor::optional_background_texture const &);

	sge::opencl::memory_object::buffer &
	cl_buffer();

	monitor::arrow_scale const
	arrow_scale() const;

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

	sge::rucksack::widget::base &
	widget();

	~arrows();
private:
	fcppt::string const name_;
	monitor::grid_dimensions::value_type const dimensions_;
	monitor::arrow_scale const arrow_scale_;
	monitor::grid_scale const grid_scale_;
	monitor::rect::vector position_;
	sge::renderer::vertex_buffer_scoped_ptr const vb_;
	sge::opencl::memory_object::buffer cl_vb_;
	fcppt::scoped_ptr<dummy_sprite::object> sprite_;
	sge::rucksack::widget::box::base box_parent_;
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
