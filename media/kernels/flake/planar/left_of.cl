#ifndef FLAKE_PLANAR_LEFT_OF_CL_INCLUDED
#define FLAKE_PLANAR_LEFT_OF_CL_INCLUDED

size_t
flake_planar_left_of(
	uint const pitch,
	int2 const rect_size,
	int2 const position)
{
	return
		pitch * position.y + max(0,position.x - 1);
}

#endif
