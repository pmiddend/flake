#ifndef FLAKELIB_VOLUME_TOP_OF_CL_INCLUDED
#define FLAKELIB_VOLUME_TOP_OF_CL_INCLUDED

size_t
flakelib_volume_top_of(
	uint const line_pitch,
	int4 const rect_size,
	int4 const position)
{
	return
		rect_size.y * line_pitch * position.z + line_pitch * max(0,position.y-1) + position.x;
}

#endif
