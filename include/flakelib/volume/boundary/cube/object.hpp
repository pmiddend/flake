#ifndef FLAKELIB_VOLUME_BOUNDARY_CUBE_OBJECT_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_CUBE_OBJECT_HPP_INCLUDED

#include <flakelib/volume/boundary/cube/position.hpp>
#include <flakelib/volume/boundary/cube/width.hpp>
#include <fcppt/math/dim/basic_impl.hpp>


namespace flakelib
{
namespace volume
{
namespace boundary
{
namespace cube
{
class object
{
public:
	explicit
	object(
		cube::width const &,
		cube::position const &);

	cube::width const &
	width() const;

	cube::position const &
	position() const;
private:
	cube::width width_;
	cube::position position_;
};
}
}
}
}

#endif
