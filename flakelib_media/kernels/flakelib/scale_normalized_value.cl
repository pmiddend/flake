#ifndef FLAKELIB_SCALE_NORMALIZED_VALUE_CL_INCLUDED
#define FLAKELIB_SCALE_NORMALIZED_VALUE_CL_INCLUDED

#define FLAKELIB_SCALE_NORMALIZED_VALUE(f,minimum,maximum)\
	(minimum) + (f) * ((maximum) - (minimum))

#endif
