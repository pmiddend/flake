#ifndef FLAKE_PLANAR_INTERPOLATE_RIGHT_NEIGHBORHOOD_CL_INCLUDED
#define FLAKE_PLANAR_INTERPOLATE_RIGHT_NEIGHBORHOOD_CL_INCLUDED

#include <flake/planar/right_neighborhood.cl>

float2
flake_planar_interpolate_right_neighborhood(
	struct flake_planar_right_neighborhood *ns,
	float2 const fractions)
{
	return
		mix(
			mix(
				ns->at,
				ns->right,
				fractions.x),
			mix(
				ns->bottom,
				ns->rightbottom,
				fractions.x),
			fractions.y);
}

#endif
