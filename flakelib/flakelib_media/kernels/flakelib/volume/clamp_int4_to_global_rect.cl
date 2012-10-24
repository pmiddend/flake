#ifndef FLAKELIB_VOLUME_CLAMP_INT4_TO_GLOBAL_RECT_CL_INCLUDED
#define FLAKELIB_VOLUME_CLAMP_INT4_TO_GLOBAL_RECT_CL_INCLUDED

int4
flakelib_volume_clamp_int4_to_global_rect(
	int4 const position)
{
	return
		clamp(
			position,
			(int4)(
				0,
				0,
				0,
				0),
			(int4)(
				get_global_size(0)-1,
				get_global_size(1)-1,
				get_global_size(2)-1,
				0));
}

#endif
