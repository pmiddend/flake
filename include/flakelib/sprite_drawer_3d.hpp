#ifndef FLAKELIB_SPRITE_DRAWER_3D_HPP_INCLUDED
#define FLAKELIB_SPRITE_DRAWER_3D_HPP_INCLUDED

#include <sge/font/dim.hpp>
#include <sge/font/image_view.hpp>
#include <sge/font/pos.hpp>
#include <sge/font/text/char_type.hpp>
#include <sge/font/text/drawer.hpp>
#include <sge/font/text/symbol.hpp>
#include <sge/image/color/bgra8_format.hpp>
#include <sge/image/color/object_impl.hpp>
#include <sge/image/color/any/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/sprite/object_decl.hpp>
#include <sge/sprite/system_decl.hpp>
#include <sge/sprite/config/choices.hpp>
#include <sge/sprite/config/float_type.hpp>
#include <sge/sprite/config/normal_size.hpp>
#include <sge/sprite/config/texture_coordinates.hpp>
#include <sge/sprite/config/texture_level_count.hpp>
#include <sge/sprite/config/type_choices.hpp>
#include <sge/sprite/config/unit_type.hpp>
#include <sge/sprite/config/with_color.hpp>
#include <sge/sprite/config/with_texture.hpp>
#include <sge/texture/const_part_ptr.hpp>
#include <sge/texture/manager.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/variant/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <map>
#include <vector>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
class sprite_drawer_3d
:
	public sge::font::text::drawer
{
FCPPT_NONCOPYABLE(
	sprite_drawer_3d);
public:
	sprite_drawer_3d(
		sge::renderer::device &,
		sge::image::color::any::object const &);

	~sprite_drawer_3d();

	void
	begin_rendering(
		sge::font::text::drawer::size_type buffer_chars,
		sge::font::pos const &start,
		sge::font::dim const &size);

	void
	draw_char(
		sge::font::text::char_type,
		sge::font::pos const &,
		sge::font::const_image_view const &);

	void
	end_rendering();

	void
	color(
		sge::image::color::any::object const &);
private:
	sge::texture::const_part_ptr const
	cached_texture(
		sge::font::text::char_type,
		sge::font::const_image_view const &);

	typedef sge::image::color::bgra8_format color_format;

	typedef sge::image::color::object<
		color_format
	>::type color_object;

	color_object col_;

	sge::texture::manager texman_;

	typedef std::map<
		sge::font::text::char_type,
		sge::texture::const_part_ptr
	> texture_map;

	texture_map textures_;

	typedef sge::sprite::config::choices<
		sge::sprite::config::type_choices<
			sge::sprite::config::unit_type<
				int
			>,
			sge::sprite::config::float_type<
				float
			>
		>,
		sge::sprite::config::normal_size,
		boost::mpl::vector2<
			sge::sprite::config::with_color<
				color_format
			>,
			sge::sprite::config::with_texture<
				sge::sprite::config::texture_level_count<
					1u
				>,
				sge::sprite::config::texture_coordinates::normal
			>
		>
	> sprite_choices;

	typedef sge::sprite::object<
		sprite_choices
	> sprite_object;

	typedef std::vector<
		sprite_object
	> sprite_container;

	typedef sge::sprite::system<
		sprite_choices
	> sprite_system;

	sprite_system sprite_system_;

	sprite_container sprites_;
};
}

#endif
