#ifndef FLAKELIB_PLANAR_BOTTOM_OF_CL_INCLUDED
#define FLAKELIB_PLANAR_BOTTOM_OF_CL_INCLUDED

size_t
flakelib_planar_bottom_of(
	uint const pitch,
	int2 const rect_size,
	int2 const position)
{
	return
		pitch * min(rect_size.x-1,position.y+1) + position.x;
}

#endif
