#ifndef FLAKE_PLANAR_AT_HPP_INCLUDED
#define FLAKE_PLANAR_AT_HPP_INCLUDED

size_t
flake_planar_at(
	uint const pitch,
	int2 const position)
{
	return
		pitch * position.y + position.x;
}

#endif
