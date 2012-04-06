#ifndef FLAKELIB_VOLUME_BOTTOM_OF_CL_INCLUDED
#define FLAKELIB_VOLUME_BOTTOM_OF_CL_INCLUDED

size_t
flakelib_volume_bottom_of(
	uint const line_pitch,
	int4 const rect_size,
	int4 const position)
{
	return
		line_pitch * rect_size.y * position.z + line_pitch * min(rect_size.y-1,position.y+1) + position.x;
}

#endif
