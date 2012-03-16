#ifndef FLAKELIB_SPLATTER_PEN_OBJECT_DECL_HPP_INCLUDED
#define FLAKELIB_SPLATTER_PEN_OBJECT_DECL_HPP_INCLUDED

#include <flakelib/splatter/pen/object_fwd.hpp>
#include <flakelib/splatter/pen/blend_factor.hpp>
#include <flakelib/splatter/pen/is_smooth.hpp>
#include <flakelib/splatter/pen/is_round.hpp>
#include <flakelib/splatter/pen/draw_mode.hpp>

namespace flakelib
{
namespace splatter
{
namespace pen
{
template<typename Bounding>
class object
{
public:
	typedef
	Bounding
	bounding_type;

	explicit
	object(
		bounding_type const &,
		pen::is_round const &,
		pen::is_smooth const &,
		pen::draw_mode::type,
		pen::blend_factor const &);

	explicit
	object(
		object const &,
		bounding_type const &);

	bounding_type const &
	bounding() const;

	pen::is_round const &
	is_round() const;

	pen::is_smooth const &
	is_smooth() const;

	pen::draw_mode::type
	draw_mode() const;

	pen::blend_factor const &
	blend_factor() const;
private:
	bounding_type bounding_;
	pen::is_round is_round_;
	pen::is_smooth is_smooth_;
	pen::draw_mode::type draw_mode_;
	pen::blend_factor blend_factor_;

};
}
}
}

#endif

