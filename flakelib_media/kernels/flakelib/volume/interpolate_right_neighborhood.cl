#ifndef FLAKELIB_VOLUME_INTERPOLATE_RIGHT_NEIGHBORHOOD_CL_INCLUDED
#define FLAKELIB_VOLUME_INTERPOLATE_RIGHT_NEIGHBORHOOD_CL_INCLUDED

#define FLAKELIB_VOLUME_INTERPOLATE_RIGHT_NEIGHBORHOOD(neighborhood,fractions)\
	mix(\
		mix(\
			mix(\
				(neighborhood).at,\
				(neighborhood).right,\
				(fractions).x),\
			mix(\
				(neighborhood).bottom,\
				(neighborhood).rightbottom,\
				(fractions).x),\
			(fractions).y),\
		mix(\
			mix(\
				(neighborhood).back,\
				(neighborhood).backright,\
				(fractions).x),\
			mix(\
				(neighborhood).backbottom,\
				(neighborhood).backrightbottom,\
				(fractions).x),\
			(fractions).y),\
		(fractions).z);

#endif
