#ifndef FLAKELIB_MONITOR_PLANAR_ARROWS_HPP_INCLUDED
#define FLAKELIB_MONITOR_PLANAR_ARROWS_HPP_INCLUDED

#include <flakelib/planar_object.hpp>
#include <flakelib/monitor/arrow_scale.hpp>
#include <flakelib/monitor/child.hpp>
#include <flakelib/monitor/grid_dimensions.hpp>
#include <flakelib/monitor/grid_scale.hpp>
#include <flakelib/monitor/name.hpp>
#include <flakelib/monitor/parent_fwd.hpp>
#include <flakelib/monitor/rect.hpp>
#include <flakelib/monitor/dummy_sprite/object.hpp>
#include <rucksack/widget/dummy.hpp>
#include <rucksack/widget/box/base.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/memory_object/image/planar_fwd.hpp>
#include <sge/renderer/vertex_buffer_ptr.hpp>
#include <sge/renderer/texture/planar_ptr.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/math/vector/basic_impl.hpp>


namespace flakelib
{
namespace monitor
{
class planar_arrows
:
	public monitor::child
{
FCPPT_NONCOPYABLE(
	planar_arrows);
public:
	explicit
	planar_arrows(
		monitor::parent &,
		monitor::name const &,
		monitor::grid_dimensions const &,
		monitor::arrow_scale const &,
		monitor::grid_scale const &,
		sge::renderer::texture::planar_ptr);

	void
	from_planar_object(
		flakelib::planar_object const &);

	fcppt::string const
	name() const;

	void
	render(
		monitor::optional_projection const &);

	void
	update();

	rucksack::widget::base &
	widget();

	~planar_arrows();
private:
	fcppt::string const name_;
	monitor::grid_dimensions::value_type const dimensions_;
	monitor::arrow_scale const arrow_scale_;
	monitor::grid_scale const grid_scale_;
	monitor::rect::vector position_;
	sge::renderer::vertex_buffer_ptr const vb_;
	sge::opencl::memory_object::buffer cl_vb_;
	fcppt::scoped_ptr<dummy_sprite::object> sprite_;
	rucksack::widget::box::base box_parent_;
	rucksack::widget::dummy font_box_;
	rucksack::widget::dummy sprite_box_;
};
}
}

#endif
