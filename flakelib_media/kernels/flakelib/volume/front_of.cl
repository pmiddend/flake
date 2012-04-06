#ifndef FLAKELIB_VOLUME_FRONT_OF_CL_INCLUDED
#define FLAKELIB_VOLUME_FRONT_OF_CL_INCLUDED

size_t
flakelib_volume_front_of(
	uint const line_pitch,
	int4 const rect_size,
	int4 const position)
{
	return
		line_pitch * rect_size.y * min(rect_size.z-1,position.z+1) + line_pitch * position.y + position.x;
}

#endif
