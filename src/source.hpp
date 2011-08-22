#ifndef FLAKE_SOURCE_HPP_INCLUDED
#define FLAKE_SOURCE_HPP_INCLUDED

#include "scalar_store.hpp"
#include "scalar.hpp"

namespace flake
{
class source
{
public:
	typedef
	flake::scalar_store::dim_type
	position_type;

	explicit
	source(
		position_type const &,
		flake::scalar);

	position_type const &
	position() const;

	flake::scalar
	intensity() const;
private:
	position_type position_;
	flake::scalar intensity_;
};
}

#endif
