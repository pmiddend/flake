#ifndef FLAKELIB_PLANAR_TOP_OF_CL_INCLUDED
#define FLAKELIB_PLANAR_TOP_OF_CL_INCLUDED

size_t
flakelib_planar_top_of(
	uint const pitch,
	int2 const rect_size,
	int2 const position)
{
	return
		pitch * max(0,position.y-1) + position.x;
}

#endif
