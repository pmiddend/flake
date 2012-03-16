#ifndef FLAKELIB_SPLATTER_PEN_OBJECT_IMPL_HPP_INCLUDED
#define FLAKELIB_SPLATTER_PEN_OBJECT_IMPL_HPP_INCLUDED

#include <flakelib/splatter/pen/object_decl.hpp>

template<typename Bounding>
flakelib::splatter::pen::object<Bounding>::object(
	bounding_type const &_bounding,
	pen::is_round const &_is_round,
	pen::is_smooth const &_is_smooth,
	pen::draw_mode::type const _draw_mode,
	pen::blend_factor const &_blend_factor)
:
	bounding_(
		_bounding),
	is_round_(
		_is_round),
	is_smooth_(
		_is_smooth),
	draw_mode_(
		_draw_mode),
	blend_factor_(
		_blend_factor)
{
}

template<typename Bounding>
flakelib::splatter::pen::object<Bounding>::object(
	object const &_object,
	bounding_type const &_bounding)
:
	bounding_(
		_bounding),
	is_round_(
		_object.is_round()),
	is_smooth_(
		_object.is_smooth()),
	draw_mode_(
		_object.draw_mode()),
	blend_factor_(
		_object.blend_factor())
{
}

template<typename Bounding>
typename
flakelib::splatter::pen::object<Bounding>::bounding_type const &
flakelib::splatter::pen::object<Bounding>::bounding() const
{
	return bounding_;
}

template<typename Bounding>
flakelib::splatter::pen::is_round const &
flakelib::splatter::pen::object<Bounding>::is_round() const
{
	return is_round_;
}

template<typename Bounding>
flakelib::splatter::pen::is_smooth const &
flakelib::splatter::pen::object<Bounding>::is_smooth() const
{
	return is_smooth_;
}

template<typename Bounding>
flakelib::splatter::pen::draw_mode::type
flakelib::splatter::pen::object<Bounding>::draw_mode() const
{
	return draw_mode_;
}

template<typename Bounding>
flakelib::splatter::pen::blend_factor const &
flakelib::splatter::pen::object<Bounding>::blend_factor() const
{
	return blend_factor_;
}

#endif
