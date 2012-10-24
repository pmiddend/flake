#ifndef FLAKELIB_VOLUME_LEFT_OF_CL_INCLUDED
#define FLAKELIB_VOLUME_LEFT_OF_CL_INCLUDED

size_t
flakelib_volume_left_of(
	uint const line_pitch,
	int4 const rect_size,
	int4 const position)
{
	return
		rect_size.y * line_pitch * position.z + line_pitch * position.y + max(0,position.x-1);
}

#endif
