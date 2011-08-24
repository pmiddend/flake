#ifndef FLAKE_SOURCE_HPP_INCLUDED
#define FLAKE_SOURCE_HPP_INCLUDED

#include "scalar.hpp"
#include <fcppt/container/grid/object_impl.hpp>

namespace flake
{
template<typename Grid>
class source
{
public:
	typedef typename
	Grid::value_type
	value_type;

	typedef typename
	Grid::dim
	position_type;

	explicit
	source(
		position_type const &_position,
		value_type const &_value)
	:
		position_(
			_position),
		value_(
			_value)
	{
	}

	position_type const &
	position() const
	{
		return
			position_;
	}

	value_type const &
	value() const
	{
		return
			value_;
	}
private:
	position_type position_;
	value_type value_;
};
}

#endif
