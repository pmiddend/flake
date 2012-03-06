#ifndef FLAKELIB_PLANAR_AT_HPP_INCLUDED
#define FLAKELIB_PLANAR_AT_HPP_INCLUDED

size_t
flakelib_planar_at(
	uint const pitch,
	int2 const position)
{
	return
		pitch * position.y + position.x;
}

#endif
