#ifndef FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION_CL_INCLUDED

#define FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION(operation,source,target)\
	operation((source).left,(target).left);\
	operation((source).right,(target).right);\
	operation((source).top,(target).top);\
	operation((source).bottom,(target).bottom);\
	operation((source).front,(target).front);\
	operation((source).back,(target).back);

#endif
