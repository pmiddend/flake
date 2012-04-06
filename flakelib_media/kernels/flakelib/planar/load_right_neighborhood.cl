#ifndef FLAKELIB_PLANAR_LOAD_RIGHT_NEIGHBORHOOD_CL_INCLUDED
#define FLAKELIB_PLANAR_LOAD_RIGHT_NEIGHBORHOOD_CL_INCLUDED

#include <flakelib/planar/right_neighborhood.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/planar/right_of.cl>
#include <flakelib/planar/bottom_of.cl>
#include <flakelib/planar/right_bottom_of.cl>

void
flakelib_planar_load_right_neighborhood(
	global float const *input,
	struct flakelib_planar_right_neighborhood *ns,
	uint const buffer_pitch,
	int2 const position,
	int2 const rect)
{
	ns->at =
		input[
			flakelib_planar_at(
				buffer_pitch,
				position)];

	ns->right =
		input[
			flakelib_planar_right_of(
				buffer_pitch,
				rect,
				position)];

	ns->bottom =
		input[
			flakelib_planar_bottom_of(
				buffer_pitch,
				rect,
				position)];

	ns->rightbottom =
		input[
			flakelib_planar_right_bottom_of(
				buffer_pitch,
				rect,
				position)];
}

#endif
