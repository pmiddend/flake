#ifndef FLAKELIB_VOLUME_AT_CL_INCLUDED
#define FLAKELIB_VOLUME_AT_CL_INCLUDED

size_t
flakelib_volume_at(
	uint const line_pitch,
	int4 const rect_size,
	int4 const position)
{
	return
		rect_size.y * line_pitch * position.z + line_pitch * position.y + position.x;
}

#endif
