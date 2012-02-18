#ifndef FLAKE_PLANAR_RIGHT_BOTTOM_OF_HPP_INCLUDED
#define FLAKE_PLANAR_RIGHT_BOTTOM_OF_HPP_INCLUDED

size_t
flake_planar_right_bottom_of(
	uint const pitch,
	int2 const rect_size,
	int2 const position)
{
	return
		pitch *
		min(rect_size.y-1,position.y+1) +
		min(rect_size.x-1,position.x+1);
}

#endif
