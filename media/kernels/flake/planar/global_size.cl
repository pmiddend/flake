#ifndef FLAKE_PLANAR_GET_GLOBAL_SIZE_HPP_INCLUDED
#define FLAKE_PLANAR_GET_GLOBAL_SIZE_HPP_INCLUDED

int2
flake_planar_global_size()
{
	return
		(int2)(
			get_global_size(0),
			get_global_size(1));
}

#endif
