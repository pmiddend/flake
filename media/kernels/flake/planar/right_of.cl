#ifndef FLAKE_PLANAR_RIGHT_OF_CL_INCLUDED
#define FLAKE_PLANAR_RIGHT_OF_CL_INCLUDED

size_t
flake_planar_right_of(
	uint const pitch,
	int2 const rect_size,
	int2 const position)
{
	return
		pitch * position.y + min(rect_size.x-1,position.x + 1);
}

#endif
