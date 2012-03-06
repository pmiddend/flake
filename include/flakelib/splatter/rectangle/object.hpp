#ifndef FLAKELIB_SPLATTER_RECTANGLE_OBJECT_HPP_INCLUDED
#define FLAKELIB_SPLATTER_RECTANGLE_OBJECT_HPP_INCLUDED

#include <flakelib/splatter/rectangle/position.hpp>
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
	explicit
	object(
		rectangle::position const &,
		rectangle::size const &);

	rectangle::position const &
	position() const;

	rectangle::size const &
	size() const;

	~object();
private:
	rectangle::position position_;
	rectangle::size size_;
};
}
}
}

#endif

