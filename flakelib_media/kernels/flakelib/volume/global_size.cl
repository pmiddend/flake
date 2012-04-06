#ifndef FLAKELIB_VOLUME_GLOBAL_SIZE_HPP_INCLUDED
#define FLAKELIB_VOLUME_GLOBAL_SIZE_HPP_INCLUDED

int4
flakelib_volume_global_size()
{
	return
		(int4)(
			get_global_size(0),
			get_global_size(1),
			get_global_size(2),
			0);
}

#endif
