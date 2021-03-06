#ifndef FLAKELIB_PLANAR_CLAMP_INT2_TO_GLOBAL_RECT_CL_INCLUDED
#define FLAKELIB_PLANAR_CLAMP_INT2_TO_GLOBAL_RECT_CL_INCLUDED

int2
flakelib_planar_clamp_int2_to_global_rect(
	int2 const position)
{
	return
		clamp(
			position,
			(int2)(0),
			(int2)(
				get_global_size(0)-1,
				get_global_size(1)-1));
}

#endif
