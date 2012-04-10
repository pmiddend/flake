#ifndef FLAKELIB_VOLUME_VON_NEUMANN_DEFINE_NEIGHBORS_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_DEFINE_NEIGHBORS_CL_INCLUDED

#define FLAKELIB_VOLUME_VON_NEUMANN_DEFINE_NEIGHBORS(inner_type,name)\
	typedef struct\
	{\
		inner_type left;\
		inner_type right;\
		inner_type top;\
		inner_type bottom;\
		inner_type front;\
		inner_type back;\
	} name

#endif
