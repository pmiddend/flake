#ifndef FLAKELIB_PLANAR_LOAD_RIGHT_NEIGHBORHOOD_CL_INCLUDED
#define FLAKELIB_PLANAR_LOAD_RIGHT_NEIGHBORHOOD_CL_INCLUDED

#include <flakelib/planar/right_neighborhood.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/planar/right_of.cl>
#include <flakelib/planar/bottom_of.cl>
#include <flakelib/planar/right_bottom_of.cl>

#define FLAKELIB_PLANAR_LOAD_RIGHT_NEIGHBORHOOD(input,neighborhood,buffer_pitch,position,rect)\
	(neighborhood).at =\
		(input)[\
			flakelib_planar_at(\
				(buffer_pitch),\
				(position))];\
	(neighborhood).right =\
		(input)[\
			flakelib_planar_right_of(\
				(buffer_pitch),\
				(rect),\
				(position))];\
	(neighborhood).bottom =\
		(input)[\
			flakelib_planar_bottom_of(\
				(buffer_pitch),\
				(rect),\
				(position))];\
	(neighborhood).rightbottom =\
		(input)[\
			flakelib_planar_right_bottom_of(\
				(buffer_pitch),\
				(rect),\
				(position))];

#endif
