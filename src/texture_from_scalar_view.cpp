#include "texture_from_scalar_view.hpp"
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
#include <fcppt/variant/apply_unary.hpp>
#include <fcppt/nonassignable.hpp>

namespace
{
template<typename AnyView>
class convert_visitor
{
FCPPT_NONASSIGNABLE(
	convert_visitor);
public:
	typedef
	void
	result_type;

	explicit
	convert_visitor(
		AnyView const &_view)
	:
		view_(
			_view)
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
	AnyView const &view_;
};
}

sge::renderer::texture::planar_ptr const
flake::texture_from_scalar_view(
	sge::renderer::device &renderer,
	flake::const_scalar_view const &view)
{
	sge::renderer::texture::planar_ptr const result =
		renderer.create_planar_texture(
			sge::renderer::texture::planar_parameters(
				sge::renderer::dim2(
					view.dim()[0],
					view.dim()[1]),
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
		convert_visitor<flake::const_scalar_view>(
			view),
		lock.value().get());

	return
		result;
}
