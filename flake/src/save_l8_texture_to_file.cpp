#include <flake/save_l8_texture_to_file.hpp>
#include <sge/image/algorithm/may_overlap.hpp>
#include <sge/image/algorithm/uninitialized.hpp>
#include <sge/image/view/wrap.hpp>
#include <sge/image2d/dim.hpp>
#include <sge/image2d/store/l8.hpp>
#include <sge/image2d/save_from_view.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/renderer/texture/const_scoped_planar_lock.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <fcppt/cast/size_fun.hpp>
#include <fcppt/math/dim/structure_cast.hpp>


void
flake::save_l8_texture_to_file(
	sge::image2d::system &_image_system,
	sge::renderer::texture::planar &_texture,
	boost::filesystem::path const &_path)
{
	typedef sge::image2d::store::l8 store_type;

	store_type const whole_store(
		fcppt::math::dim::structure_cast<
			sge::image2d::dim,
			fcppt::cast::size_fun>(
			_texture.size()),
		[
			&_texture
		](
			store_type::view_type const &_view)
		{
			sge::renderer::texture::const_scoped_planar_lock slock(
				_texture);

			sge::image2d::algorithm::copy_and_convert(
				slock.value(),
				sge::image2d::view::object(
					sge::image::view::wrap(
						_view)),
				sge::image::algorithm::may_overlap::no,
				sge::image::algorithm::uninitialized::yes);
		});

	sge::image2d::save_from_view(
		_image_system,
		sge::image2d::view::const_object(
			whole_store.const_wrapped_view()),
		_path);
}
