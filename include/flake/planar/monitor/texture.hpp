#ifndef FLAKE_PLANAR_MONITOR_TEXTURE_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_TEXTURE_HPP_INCLUDED

#include <flake/planar/monitor/child.hpp>
#include <flake/planar/monitor/grid_dimensions.hpp>
#include <flake/planar/monitor/name.hpp>
#include <flake/planar/monitor/parent_fwd.hpp>
#include <flake/planar/monitor/scaling_factor.hpp>
#include <flake/planar/monitor/texture_size.hpp>
#include <flake/planar/monitor/dummy_sprite/object.hpp>
#include <rucksack/widget/dummy.hpp>
#include <rucksack/widget/box/base.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/sprite/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/box/basic_impl.hpp>


namespace flake
{
namespace planar
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
		monitor::texture_size const &,
		monitor::scaling_factor const &);

	sge::opencl::memory_object::image::planar &
	cl_texture();

	monitor::scalar
	scaling_factor() const;

	fcppt::string const
	name() const;

	void
	render(
		monitor::optional_projection const &);

	void
	update();

	rucksack::widget::base &
	widget();

	rucksack::widget::base const &
	widget() const;

	rucksack::widget::base &
	content_widget();

	rucksack::widget::base const &
	content_widget() const;

	~texture();
private:
	fcppt::string const name_;
	monitor::scalar const scaling_;
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
