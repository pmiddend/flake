#ifndef FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION_WITH_HELPER_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION_WITH_HELPER_CL_INCLUDED

#define FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION_WITH_HELPER(operation,source,target,helper)\
	operation((source).left,(target).left,helper)\
	operation((source).right,(target).right,helper)\
	operation((source).top,(target).top,helper)\
	operation((source).bottom,(target).bottom,helper)\
	operation((source).front,(target).front,helper)\
	operation((source).back,(target).back,helper)

#endif
