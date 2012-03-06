#ifndef FLAKELIB_SPLATTER_CIRCLE_OBJECT_HPP_INCLUDED
#define FLAKELIB_SPLATTER_CIRCLE_OBJECT_HPP_INCLUDED

#include <flakelib/splatter/circle/position.hpp>
#include <flakelib/splatter/circle/radius.hpp>
#include <fcppt/math/dim/basic_impl.hpp>

namespace flakelib
{
namespace splatter
{
namespace circle
{
class object
{
public:
	explicit
	object(
		circle::position const &,
		circle::radius const &);

	circle::position const &
	position() const;

	circle::radius const &
	radius() const;

	~object();
private:
	circle::position position_;
	circle::radius radius_;
};
}
}
}

#endif

