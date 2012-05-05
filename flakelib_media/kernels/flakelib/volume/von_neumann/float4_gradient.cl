#ifndef FLAKELIB_VOLUME_VON_NEUMANN_FLOAT4_GRADIENT_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_FLOAT4_GRADIENT_CL_INCLUDED

#include <flakelib/volume/von_neumann/float4_neighbors.cl>

float4
flakelib_volume_von_neumann_float4_gradient(
	flakelib_volume_von_neumann_float4_neighbors *n)
{
	return
		0.5f *
		(float4)(
			n->right.x - n->left.x,
			n->bottom.y - n->top.y,
			n->front.z - n->back.z,
			0.0f);
}

#endif
