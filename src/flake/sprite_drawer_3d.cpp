#include <flake/sprite_drawer_3d.hpp>
#include <sge/image/color/any/convert.hpp>
#include <sge/image2d/dim.hpp>
#include <sge/image2d/view/size.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/caps/object.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/texture/mipmap/off.hpp>
#include <sge/sprite/object_impl.hpp>
#include <sge/sprite/parameters_impl.hpp>
#include <sge/sprite/compare/default.hpp>
#include <sge/sprite/geometry/make_random_access_range.hpp>
#include <sge/sprite/process/default_geometry_options.hpp>
#include <sge/sprite/process/options.hpp>
#include <sge/sprite/process/with_options.hpp>
#include <sge/sprite/render/options.hpp>
#include <sge/texture/rect_fragmented.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/math/box/object_impl.hpp>
#include <fcppt/math/dim/fill.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/spirit/home/phoenix/object/new.hpp>
#include <utility>
#include <fcppt/config/external_end.hpp>


flake::sprite_drawer_3d::sprite_drawer_3d(
	sge::renderer::device &_renderer,
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
					_renderer
				),
				_renderer.caps().preferred_texture_format().get(),
				sge::renderer::texture::mipmap::off(),
				fcppt::math::dim::fill<
					2,
					sge::renderer::dim2::value_type
				>(
					256
				)
			)
		)
	),
	textures_(),
	sprite_system_(
		_renderer,
		sge::sprite::buffers::option::dynamic
	),
	sprites_()
{
}

flake::sprite_drawer_3d::~sprite_drawer_3d()
{
}

void
flake::sprite_drawer_3d::begin_rendering(
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
flake::sprite_drawer_3d::draw_char(
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
					sge::texture::const_part_shared_ptr()
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
		)
	);
}

void
flake::sprite_drawer_3d::end_rendering()
{
	sge::sprite::process::with_options
	<
		sge::sprite::process::options
		<
			sge::sprite::process::default_geometry_options
			<
				sprite_choices,
				sge::sprite::compare::default_
			>::value,
			sge::sprite::render::options
			<
				sge::sprite::render::matrix_options::nothing,
				sge::sprite::render::state_options::set,
				sge::sprite::render::vertex_options::declaration_and_buffer
			>
		>
	>(
		sge::sprite::geometry::make_random_access_range(
			sprites_.begin(),
			sprites_.end()
		),
		sprite_system_,
		sge::sprite::compare::default_());
}

void
flake::sprite_drawer_3d::color(
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

sge::texture::const_part_shared_ptr const
flake::sprite_drawer_3d::cached_texture(
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
					sge::texture::const_part_shared_ptr(
						texman_.add(
							_data
						))
				)
			).first->second;
}
