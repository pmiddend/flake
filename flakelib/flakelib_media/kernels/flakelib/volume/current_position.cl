#ifndef FLAKELIB_VOLUME_CURRENT_POSITION_HPP_INCLUDED
#define FLAKELIB_VOLUME_CURRENT_POSITION_HPP_INCLUDED

int4
flakelib_volume_current_position()
{
	return
		(int4)(
			get_global_id(0),
			get_global_id(1),
			get_global_id(2),
			0);
}

#endif
