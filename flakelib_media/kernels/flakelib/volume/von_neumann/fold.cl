#ifndef FLAKELIB_VOLUME_VON_NEUMANN_FOLD_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_FOLD_CL_INCLUDED

#define FLAKELIB_VOLUME_VON_NEUMANN_FOLD(operation,ns)\
	(operation(\
		(ns).left,\
		operation(\
			(ns).right,\
			operation(\
				(ns).top,\
				operation(\
					(ns).bottom,\
					operation(\
						(ns).front,\
						(ns).back))))))\

#endif
