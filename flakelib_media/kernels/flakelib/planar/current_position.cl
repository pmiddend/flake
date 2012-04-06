#ifndef FLAKELIB_PLANAR_CURRENT_POSITION_CL_INCLUDED
#define FLAKELIB_PLANAR_CURRENT_POSITION_CL_INCLUDED

int2
flakelib_planar_current_position()
{
	return
		(int2)(
			get_global_id(0),
			get_global_id(1));
}

#endif
