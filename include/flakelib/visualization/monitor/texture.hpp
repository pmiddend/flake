#ifndef FLAKELIB_VISUALIZATION_MONITOR_TEXTURE_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_MONITOR_TEXTURE_HPP_INCLUDED

#include <flakelib/planar_object.hpp>
#include <flakelib/visualization/monitor/child.hpp>
#include <flakelib/visualization/monitor/dim.hpp>
#include <flakelib/visualization/monitor/grid_dimensions.hpp>
#include <flakelib/visualization/monitor/name.hpp>
#include <flakelib/visualization/monitor/parent_fwd.hpp>
#include <flakelib/visualization/monitor/scaling_factor.hpp>
#include <flakelib/visualization/monitor/dummy_sprite/object.hpp>
#include <rucksack/widget/box/base.hpp>
#include <rucksack/widget/dummy.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/sprite/object.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/noncopyable.hpp>


namespace flakelib
{
namespace visualization
{
namespace monitor
{
class texture
:
	public monitor::child
{
FCPPT_NONCOPYABLE(
	texture);
public:
	explicit
	texture(
		monitor::parent &,
		monitor::name const &,
		monitor::grid_dimensions const &,
		monitor::dim const &,
		monitor::scaling_factor const &);

	void
	from_planar_object(
		flakelib::planar_object const &);

	fcppt::string const
	name() const;

	void
	render();

	void
	update();

	rucksack::widget::base &
	widget();

	~texture();
private:
	fcppt::string const name_;
	monitor::scaling_factor::value_type const scaling_;
	sge::renderer::texture::planar_ptr renderer_texture_;
	sge::opencl::memory_object::image::planar cl_texture_;
	dummy_sprite::object sprite_;
	rucksack::widget::box::base box_parent_;
	rucksack::widget::dummy font_box_;
	rucksack::widget::dummy sprite_box_;
};
}
}
}

#endif
