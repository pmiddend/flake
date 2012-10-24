#ifndef FLAKELIB_PLANAR_DEFINE_RIGHT_NEIGHBORHOOD_CL_INCLUDED
#define FLAKELIB_PLANAR_DEFINE_RIGHT_NEIGHBORHOOD_CL_INCLUDED

#define FLAKELIB_PLANAR_DEFINE_RIGHT_NEIGHBORHOOD(struct_name,inner_type)\
typedef struct\
{\
	inner_type at;\
	inner_type right;\
	inner_type bottom;\
	inner_type rightbottom;\
} struct_name

#endif
