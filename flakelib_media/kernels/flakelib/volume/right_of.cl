#ifndef FLAKELIB_VOLUME_RIGHT_OF_CL_INCLUDED
#define FLAKELIB_VOLUME_RIGHT_OF_CL_INCLUDED

size_t
flakelib_volume_right_of(
	uint const line_pitch,
	int4 const rect_size,
	int4 const position)
{
	return
		rect_size.y * line_pitch * position.z + line_pitch * position.y + min(rect_size.x-1,position.x+1);
}

#endif
