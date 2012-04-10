#include <flake/save_l8_texture_to_file.hpp>
#include <sge/image/store.hpp>
#include <sge/image/algorithm/may_overlap.hpp>
#include <sge/image2d/dim.hpp>
#include <sge/image2d/l8.hpp>
#include <sge/image2d/save_from_view.hpp>
#include <sge/image2d/algorithm/copy_and_convert.hpp>
#include <sge/image2d/view/object.hpp>
#include <sge/image2d/view/to_const.hpp>
#include <sge/renderer/texture/const_scoped_planar_lock.hpp>
#include <sge/renderer/texture/planar.hpp>
#include <fcppt/math/dim/structure_cast.hpp>


void
flake::save_l8_texture_to_file(
	sge::image2d::system &_image_system,
	sge::renderer::texture::planar &_texture,
	boost::filesystem::path const &_path)
{
	sge::renderer::texture::const_scoped_planar_lock slock(
		_texture);

	typedef sge::image2d::l8 store_type;

	store_type whole_store(
		fcppt::math::dim::structure_cast<sge::image2d::dim>(
			_texture.size()));

	sge::image2d::algorithm::copy_and_convert(
		slock.value(),
		sge::image2d::view::object(
			whole_store.wrapped_view()),
		sge::image::algorithm::may_overlap::no);

	sge::image2d::save_from_view(
		_image_system,
		sge::image2d::view::to_const(
			sge::image2d::view::object(
				whole_store.wrapped_view())),
		_path);
}
