#ifndef FLAKELIB_VOLUME_LOAD_RIGHT_NEIGHBORHOOD_CL_INCLUDED
#define FLAKELIB_VOLUME_LOAD_RIGHT_NEIGHBORHOOD_CL_INCLUDED

#include <flakelib/volume/at.cl>
#include <flakelib/volume/right_of.cl>
#include <flakelib/volume/bottom_of.cl>
#include <flakelib/volume/right_bottom_of.cl>

#define FLAKELIB_VOLUME_LOAD_RIGHT_NEIGHBORHOOD(input,neighborhood,buffer_pitch,position,rect)\
	{\
		int4 const flakelib_volume_load_right_neighborhood_position = convert_int4(position);\
		int4 const flakelib_volume_load_right_neighborhood_rect = convert_int4(rect);\
		(neighborhood).at =\
			(input)[\
				flakelib_volume_at(\
					(buffer_pitch),\
					flakelib_volume_load_right_neighborhood_rect,\
					flakelib_volume_load_right_neighborhood_position)];\
		(neighborhood).right =\
			(input)[\
				flakelib_volume_right_of(\
					(buffer_pitch),\
					flakelib_volume_load_right_neighborhood_rect,\
					flakelib_volume_load_right_neighborhood_position)];\
		(neighborhood).bottom =\
			(input)[\
				flakelib_volume_bottom_of(\
					(buffer_pitch),\
					flakelib_volume_load_right_neighborhood_rect,\
					flakelib_volume_load_right_neighborhood_position)];\
		(neighborhood).rightbottom =\
			(input)[\
				flakelib_volume_right_bottom_of(\
					(buffer_pitch),\
					flakelib_volume_load_right_neighborhood_rect,\
					flakelib_volume_load_right_neighborhood_position)];\
		int4 const flakelib_volume_load_right_neighborhood_front_position = (int4)(\
			flakelib_volume_load_right_neighborhood_position.x,\
			flakelib_volume_load_right_neighborhood_position.y,\
			min(\
				flakelib_volume_load_right_neighborhood_rect.z-1,\
				flakelib_volume_load_right_neighborhood_position.z+1),\
			0);\
		(neighborhood).back =\
			(input)[\
				flakelib_volume_at(\
					(buffer_pitch),\
					flakelib_volume_load_right_neighborhood_rect,\
					flakelib_volume_load_right_neighborhood_front_position)];\
		(neighborhood).backright =\
			(input)[\
				flakelib_volume_right_of(\
					(buffer_pitch),\
					flakelib_volume_load_right_neighborhood_rect,\
					flakelib_volume_load_right_neighborhood_front_position)];\
		(neighborhood).backbottom =\
			(input)[\
				flakelib_volume_bottom_of(\
					(buffer_pitch),\
					flakelib_volume_load_right_neighborhood_rect,\
					flakelib_volume_load_right_neighborhood_front_position)];\
		(neighborhood).backrightbottom =\
			(input)[\
				flakelib_volume_right_bottom_of(\
					(buffer_pitch),\
					flakelib_volume_load_right_neighborhood_rect,\
					flakelib_volume_load_right_neighborhood_front_position)];\
	}

#endif
