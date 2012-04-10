#ifndef FLAKELIB_VOLUME_VON_NEUMANN_MIX_WITH_CENTER_VALUE_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_MIX_WITH_CENTER_VALUE_CL_INCLUDED

#include <flakelib/volume/von_neumann/float_neighbors.cl>
#include <flakelib/volume/von_neumann/size_t_neighbors.cl>
#include <flakelib/volume/von_neumann/apply_binary_operation.cl>

flakelib_volume_von_neumann_float_neighbors
flakelib_volume_von_neumann_mix_with_center_value(
	flakelib_volume_von_neumann_size_t_neighbors *neighbors,
	global float const *boundary,
	global float const *input,
	float const center)
{
	flakelib_volume_von_neumann_float_neighbors result;

	#define FLAKELIB_VOLUME_VON_NEUMANN_MIX_WITH_CENTER_VALUE_OPERATION(source,target)\
	target = mix(input[source],center,boundary[source])

	FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION(
		FLAKELIB_VOLUME_VON_NEUMANN_MIX_WITH_CENTER_VALUE_OPERATION,
		*neighbors,
		result);

	return result;
}

#endif
