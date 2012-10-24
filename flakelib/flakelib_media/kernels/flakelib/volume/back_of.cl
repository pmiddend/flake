#ifndef FLAKELIB_VOLUME_BACK_OF_CL_INCLUDED
#define FLAKELIB_VOLUME_BACK_OF_CL_INCLUDED

size_t
flakelib_volume_back_of(
	uint const line_pitch,
	int4 const rect_size,
	int4 const position)
{
	return
		line_pitch * rect_size.y * max(0,position.z-1) + line_pitch * position.y + position.x;
}

#endif
