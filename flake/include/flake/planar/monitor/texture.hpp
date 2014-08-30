#ifndef FLAKE_PLANAR_MONITOR_TEXTURE_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_TEXTURE_HPP_INCLUDED

#include <flake/planar/monitor/child.hpp>
#include <flake/planar/monitor/grid_dimensions.hpp>
#include <flake/planar/monitor/name.hpp>
#include <flake/planar/monitor/parent_fwd.hpp>
#include <flake/planar/monitor/scaling_factor.hpp>
#include <flake/planar/monitor/texture_size.hpp>
#include <flake/planar/monitor/dummy_sprite/object.hpp>
#include <sge/font/draw/static_text.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/renderer/texture/planar_unique_ptr.hpp>
#include <sge/rucksack/widget/box.hpp>
#include <sge/rucksack/widget/dummy.hpp>
#include <sge/sprite/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/box/object_impl.hpp>


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
		sge::renderer::context::ffp &,
		monitor::optional_projection const &);

	void
	update();

	sge::rucksack::widget::base &
	widget();

	sge::rucksack::widget::base const &
	widget() const;

	sge::rucksack::widget::base &
	content_widget();

	sge::rucksack::widget::base const &
	content_widget() const;

	~texture();
private:
	fcppt::string const name_;
	monitor::scalar const scaling_;
	sge::renderer::texture::planar_unique_ptr renderer_texture_;
	sge::opencl::memory_object::image::planar cl_texture_;
	dummy_sprite::object sprite_;
	sge::rucksack::widget::box box_parent_;
	sge::rucksack::widget::dummy font_box_;
	sge::rucksack::widget::dummy sprite_box_;
	sge::font::draw::static_text font_renderable_;
};
}
}
}

#endif
