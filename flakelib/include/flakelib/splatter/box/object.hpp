#ifndef FLAKELIB_SPLATTER_BOX_OBJECT_HPP_INCLUDED
#define FLAKELIB_SPLATTER_BOX_OBJECT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/splatter/box/position.hpp>
#include <flakelib/splatter/box/size.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/vector/object_impl.hpp>

namespace flakelib
{
namespace splatter
{
namespace box
{
class object
{
public:
	FLAKELIB_SYMBOL
	explicit
	object(
		box::position const &,
		box::size const &);

	FLAKELIB_SYMBOL
	box::position const &
	position() const;

	FLAKELIB_SYMBOL
	box::size const &
	size() const;

	FLAKELIB_SYMBOL
	~object();
private:
	box::position position_;
	box::size size_;
};
}
}
}

#endif

