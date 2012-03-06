#ifndef FLAKELIB_SPLATTER_RECTANGLE_OBJECT_HPP_INCLUDED
#define FLAKELIB_SPLATTER_RECTANGLE_OBJECT_HPP_INCLUDED

#include <flakelib/splatter/rectangle/position.hpp>
#include <flakelib/symbol.hpp>
#include <flakelib/splatter/rectangle/size.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

namespace flakelib
{
namespace splatter
{
namespace rectangle
{
class object
{
public:
	FLAKELIB_SYMBOL
	explicit
	object(
		rectangle::position const &,
		rectangle::size const &);

	FLAKELIB_SYMBOL
	rectangle::position const &
	position() const;

	FLAKELIB_SYMBOL
	rectangle::size const &
	size() const;

	FLAKELIB_SYMBOL
	~object();
private:
	rectangle::position position_;
	rectangle::size size_;
};
}
}
}

#endif

