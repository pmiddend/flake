#include "texture_from_scalar_grid.hpp"
#include <sge/renderer/device.hpp>
#include <sge/renderer/texture/planar_parameters.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/renderer/texture/address_mode.hpp>
#include <sge/renderer/texture/capabilities_field.hpp>
#include <sge/renderer/texture/address_mode2.hpp>
#include <sge/renderer/texture/scoped_planar_lock.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/image/color/format.hpp>
#include <mizuiro/image/algorithm/copy_and_convert.hpp>
#include <mizuiro/image/format.hpp>
#include <mizuiro/image/dimension.hpp>
#include <mizuiro/image/view.hpp>
#include <mizuiro/access/normal.hpp>
#include <mizuiro/const_tag.hpp>
#include <mizuiro/image/interleaved.hpp>
#include <mizuiro/color/homogenous_static.hpp>
#include <mizuiro/color/layout/gray.hpp>
#include <fcppt/variant/apply_unary.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/container/grid/object.hpp>
#include <fcppt/nonassignable.hpp>

namespace
{
class convert_visitor
{
FCPPT_NONASSIGNABLE(
	convert_visitor);
public:
	typedef
	void
	result_type;

	typedef
	mizuiro::image::format
	<
		mizuiro::image::dimension<2>,
		mizuiro::image::interleaved
		<
			mizuiro::color::homogenous_static
			<
				flake::scalar,
				mizuiro::color::layout::gray
			>
		>
	>
	target_color_format;

	typedef
	mizuiro::image::view
	<
		mizuiro::access::normal,
		target_color_format,
		mizuiro::const_tag
	>
	view_type;

	explicit
	convert_visitor(
		flake::scalar_grid2 const &_grid)
	:
		view_(
			view_type(
				view_type::dim_type(
					_grid.size()[0],
					_grid.size()[1]),
				_grid.data()))
	{
	}

	template<typename OtherView>
	result_type
	operator()(
		OtherView const &other) const
	{
		mizuiro::image::algorithm::copy_and_convert(
			view_,
			other);
	}
private:
	view_type const view_;
};
}

sge::renderer::texture::planar_ptr const
flake::texture_from_scalar_grid(
	sge::renderer::device &renderer,
	flake::scalar_grid2 const &view)
{
	sge::renderer::texture::planar_ptr const result =
		renderer.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				fcppt::math::dim::structure_cast<sge::renderer::dim2>(
					view.size()),
				sge::image::color::format::gray8,
				sge::renderer::texture::mipmap::off(),
				sge::renderer::texture::address_mode2(
					sge::renderer::texture::address_mode::clamp),
				sge::renderer::resource_flags::readable,
				sge::renderer::texture::capabilities_field()));

	sge::renderer::texture::scoped_planar_lock lock(
		*result,
		sge::renderer::lock_mode::readwrite);

	fcppt::variant::apply_unary(
		convert_visitor(
			view),
		lock.value().get());

	return
		result;
}
