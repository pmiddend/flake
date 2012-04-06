#ifndef FLAKELIB_VOLUME_RIGHT_BOTTOM_OF_CL_INCLUDED
#define FLAKELIB_VOLUME_RIGHT_BOTTOM_OF_CL_INCLUDED

size_t
flakelib_volume_right_bottom_of(
	uint const line_pitch,
	int4 const rect_size,
	int4 const position)
{
	return
		rect_size.y * line_pitch * position.z + line_pitch * min(rect_size.y-1,position.y+1) + min(rect_size.x-1,position.x+1);
}

#endif
