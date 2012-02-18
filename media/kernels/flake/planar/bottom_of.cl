#ifndef FLAKE_PLANAR_BOTTOM_OF_HPP_INCLUDED
#define FLAKE_PLANAR_BOTTOM_OF_HPP_INCLUDED

size_t
flake_planar_bottom_of(
	uint const pitch,
	int2 const rect_size,
	int2 const position)
{
	return
		pitch * min(rect_size.x-1,position.y+1) + position.x;
}

#endif
