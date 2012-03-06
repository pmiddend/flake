#ifndef FLAKELIB_PLANAR_INTERPOLATE_RIGHT_NEIGHBORHOOD_CL_INCLUDED
#define FLAKELIB_PLANAR_INTERPOLATE_RIGHT_NEIGHBORHOOD_CL_INCLUDED

#include <flakelib/planar/right_neighborhood.cl>

float
flakelib_planar_interpolate_right_neighborhood(
	struct flakelib_planar_right_neighborhood *ns,
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
