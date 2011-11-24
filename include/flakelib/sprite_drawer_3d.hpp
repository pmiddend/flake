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
#include <sge/sprite/choices.hpp>
#include <sge/sprite/external_system_decl.hpp>
#include <sge/sprite/object_decl.hpp>
#include <sge/sprite/system.hpp>
#include <sge/sprite/type_choices.hpp>
#include <sge/sprite/with_color.hpp>
#include <sge/sprite/with_dim.hpp>
#include <sge/sprite/with_texture.hpp>
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
	typedef
	sge::image::color::bgra8_format
	color_format;

	typedef
	sge::image::color::object<color_format>::type
	color_object;

	typedef
	sge::sprite::choices
	<
		sge::sprite::type_choices
		<
			int,
			float,
			color_format
		>,
		boost::mpl::vector3<
			sge::sprite::with_color,
			sge::sprite::with_dim,
			sge::sprite::with_texture
		>
	>
	sprite_choices;

	typedef
	sge::sprite::system<sprite_choices>::type
	sprite_system;

	typedef
	sge::sprite::object<sprite_choices>
	sprite_object;

	typedef
	std::vector<sprite_object>
	sprite_container;

	explicit
	sprite_drawer_3d(
		sge::renderer::device &,
		sge::image::color::any::object const &);

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

	~sprite_drawer_3d();
private:
	typedef
	std::map
	<
		sge::font::text::char_type,
		sge::texture::const_part_ptr
	>
	texture_map;


	sge::texture::const_part_ptr const
	cached_texture(
		sge::font::text::char_type,
		sge::font::const_image_view const &
	);

	color_object col_;
	sge::texture::manager texman_;
	texture_map textures_;
	sprite_system sys_;
	sprite_container sprites_;
};
}

#endif
