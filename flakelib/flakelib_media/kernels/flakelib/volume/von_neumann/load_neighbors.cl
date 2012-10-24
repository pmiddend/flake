#ifndef FLAKELIB_VOLUME_VON_NEUMANN_LOAD_NEIGHBORS_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_LOAD_NEIGHBORS_CL_INCLUDED

#include <flakelib/volume/von_neumann/size_t_neighbors.cl>
#include <flakelib/volume/von_neumann/apply_binary_operation_with_helper.cl>
#include <flakelib/volume/von_neumann/load_neighbor_indices.cl>

#define FLAKELIB_VOLUME_VON_NEUMANN_LOAD_NEIGHBORS_LOAD_OPERATION(source,target,inputsource)\
	target = inputsource[source];

#define FLAKELIB_VOLUME_VON_NEUMANN_LOAD_NEIGHBORS(source,destination,position,rect,pitch)\
	{\
		flakelib_volume_von_neumann_size_t_neighbors neighborhood =\
			flakelib_volume_von_neumann_load_neighbor_indices(\
				(position),\
				(rect),\
				(pitch));\
\
		FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION_WITH_HELPER(\
			FLAKELIB_VOLUME_VON_NEUMANN_LOAD_NEIGHBORS_LOAD_OPERATION,\
			neighborhood,\
			(destination),\
			(source))\
	}

#endif
