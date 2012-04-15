#ifndef FLAKELIB_NORMALIZE_FROM_RANGE_CL_INCLUDED
#define FLAKELIB_NORMALIZE_FROM_RANGE_CL_INCLUDED

#define FLAKELIB_NORMALIZE_FROM_RANGE(f,minimum,maximum)\
	((f) + (minimum)) / ((maximum) - (minimum))

#endif
