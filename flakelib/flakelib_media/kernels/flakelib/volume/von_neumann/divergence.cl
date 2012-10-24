#ifndef FLAKELIB_VOLUME_VON_NEUMANN_DIVERGENCE_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_DIVERGENCE_CL_INCLUDED

#include <flakelib/volume/von_neumann/float4_neighbors.cl>

float
flakelib_volume_von_neumann_divergence(
	flakelib_volume_von_neumann_float4_neighbors *neighbors)
{
	return
		((neighbors->right.x - neighbors->left.x) + (neighbors->bottom.y - neighbors->top.y) + (neighbors->front.z - neighbors->back.z)) / 2.0f;
}

#endif
