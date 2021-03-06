#ifndef FLAKELIB_VOLUME_RIGHT_NEIGHBORS_DEFINE_CL_INCLUDED
#define FLAKELIB_VOLUME_RIGHT_NEIGHBORS_DEFINE_CL_INCLUDED

#define FLAKELIB_VOLUME_RIGHT_NEIGHBORS_DEFINE(struct_name,inner_type)\
typedef struct\
{\
	inner_type at;\
	inner_type right;\
	inner_type bottom;\
	inner_type rightbottom;\
	inner_type back;\
	inner_type backright;\
	inner_type backbottom;\
	inner_type backrightbottom;\
} struct_name

#endif
