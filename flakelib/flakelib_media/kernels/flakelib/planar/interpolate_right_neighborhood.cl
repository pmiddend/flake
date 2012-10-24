#ifndef FLAKELIB_PLANAR_INTERPOLATE_RIGHT_NEIGHBORHOOD_CL_INCLUDED
#define FLAKELIB_PLANAR_INTERPOLATE_RIGHT_NEIGHBORHOOD_CL_INCLUDED

#define FLAKELIB_PLANAR_INTERPOLATE_RIGHT_NEIGHBORHOOD(neighborhood,fractions)\
	mix(\
		mix(\
			(neighborhood).at,\
			(neighborhood).right,\
			(fractions).x),\
		mix(\
			(neighborhood).bottom,\
			(neighborhood).rightbottom,\
			(fractions).x),\
		(fractions).y)

#endif
