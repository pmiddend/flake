#ifndef FLAKELIB_PLANAR_CURRENT_POSITION_HPP_INCLUDED
#define FLAKELIB_PLANAR_CURRENT_POSITION_HPP_INCLUDED

int2
flakelib_planar_current_position()
{
	return
		(int2)(
			get_global_id(0),
			get_global_id(1));
}

#endif
