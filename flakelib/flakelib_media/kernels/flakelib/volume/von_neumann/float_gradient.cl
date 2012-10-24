#ifndef FLAKELIB_VOLUME_VON_NEUMANN_FLOAT_GRADIENT_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_FLOAT_GRADIENT_CL_INCLUDED

#include <flakelib/volume/von_neumann/float_neighbors.cl>

float4
flakelib_volume_von_neumann_float_gradient(
	flakelib_volume_von_neumann_float_neighbors *n)
{
	return
		0.5f *
		(float4)(
			n->right - n->left,
			n->bottom - n->top,
			n->front - n->back,
			0.0f);
}

#endif
