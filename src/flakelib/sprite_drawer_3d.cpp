#include <flakelib/sprite_drawer_3d.hpp>
#include <sge/image/color/any/convert.hpp>
#include <sge/image2d/dim.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/renderer/caps/object.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/sprite/default_equal.hpp>
#include <sge/sprite/default_sort.hpp>
#include <sge/sprite/external_system_impl.hpp>
#include <sge/sprite/object_impl.hpp>
#include <sge/sprite/parameters_impl.hpp>
#include <sge/sprite/render_states.hpp>
#include <sge/texture/rect_fragmented.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/math/dim/quad.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/spirit/home/phoenix/object/new.hpp>
#include <utility>
#include <fcppt/config/external_end.hpp>


flakelib::sprite_drawer_3d::sprite_drawer_3d(
	sge::renderer::device &_rend,
	sge::image::color::any::object const &_col
)
:
	col_(
		sge::image::color::any::convert<
			color_format
		>(
			_col
		)
	),
	texman_(
		boost::phoenix::construct<
			sge::texture::fragmented_unique_ptr
		>(
			boost::phoenix::new_<
				sge::texture::rect_fragmented
			>(
				fcppt::ref(
					_rend
				),
				_rend.caps().preferred_texture_format().get(),
				sge::renderer::texture::mipmap::off(),
				fcppt::math::dim::quad<
					sge::renderer::dim2
				>(
					256
				)
			)
		)
	),
	sys_(
		_rend
	),
	sprites_()
{
}

flakelib::sprite_drawer_3d::~sprite_drawer_3d()
{
}

void
flakelib::sprite_drawer_3d::begin_rendering(
	sge::font::text::drawer::size_type const buffer_chars,
	sge::font::pos const &,
	sge::font::dim const &
)
{
	sprites_.clear();

	sprites_.reserve(
		buffer_chars
	);
}

void
flakelib::sprite_drawer_3d::draw_char(
	sge::font::text::char_type const _char,
	sge::font::pos const &_pos,
	sge::font::const_image_view const &_data
)
{
	sge::image2d::dim const dim(
		sge::image2d::view::size(
			_data
		)
	);

	typedef sge::sprite::parameters<
		sprite_choices
	> sprite_parameters;

	sprites_.push_back(
		sprite_object(
			sprite_parameters()
			.pos(
				_pos
			)
			.texture(
				dim.content()
				?
					this->cached_texture(
						_char,
						_data
					)
				:
					sge::texture::const_part_ptr()
			)
			.size(
				fcppt::math::dim::structure_cast<
					sprite_object::dim
				>(
					dim
				)
			)
			.color(
				col_
			)
			.elements()
		)
	);
}

void
flakelib::sprite_drawer_3d::end_rendering()
{
	sys_.render_advanced(
		sprites_.begin(),
		sprites_.end(),
		sge::sprite::default_sort(),
		sge::sprite::default_equal()
	);
}

void
flakelib::sprite_drawer_3d::color(
	sge::image::color::any::object const &_col
)
{
	col_ =
		sge::image::color::any::convert<
			color_format
		>(
			_col
		);
}

sge::texture::const_part_ptr const
flakelib::sprite_drawer_3d::cached_texture(
	sge::font::text::char_type const _ch,
	sge::font::const_image_view const &_data
)
{
	texture_map::const_iterator const it(
		textures_.find(
			_ch
		)
	);

	return
		it != textures_.end()
		?
			it->second
		:
			textures_.insert(
				std::make_pair(
					_ch,
					texman_.add(
						_data
					)
				)
			).first->second;
}

