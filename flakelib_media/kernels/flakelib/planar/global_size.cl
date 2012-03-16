#ifndef FLAKELIB_PLANAR_GET_GLOBAL_SIZE_HPP_INCLUDED
#define FLAKELIB_PLANAR_GET_GLOBAL_SIZE_HPP_INCLUDED

int2
flakelib_planar_global_size()
{
	return
		(int2)(
			get_global_size(0),
			get_global_size(1));
}

#endif
