#ifndef FLAKE_PLANAR_CURRENT_POSITION_HPP_INCLUDED
#define FLAKE_PLANAR_CURRENT_POSITION_HPP_INCLUDED

int2
flake_planar_current_position()
{
	return
		(int2)(
			get_global_id(0),
			get_global_id(1));
}

#endif
