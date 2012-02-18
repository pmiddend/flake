#ifndef FLAKE_PLANAR_LOAD_RIGHT_NEIGHBORHOOD_HPP_INCLUDED
#define FLAKE_PLANAR_LOAD_RIGHT_NEIGHBORHOOD_HPP_INCLUDED

#include <flake/planar/right_neighborhood.cl>
#include <flake/planar/at.cl>
#include <flake/planar/right_of.cl>
#include <flake/planar/bottom_of.cl>
#include <flake/planar/right_bottom_of.cl>

void
flake_planar_load_right_neighborhood(
	global float2 const *input,
	struct flake_planar_right_neighborhood *ns,
	uint const buffer_pitch,
	int2 const position,
	int2 const rect)
{
	ns->at =
		input[
			flake_planar_at(
				buffer_pitch,
				position)];

	ns->right =
		input[
			flake_planar_right_of(
				buffer_pitch,
				rect,
				position)];

	ns->bottom =
		input[
			flake_planar_bottom_of(
				buffer_pitch,
				rect,
				position)];

	ns->rightbottom =
		input[
			flake_planar_right_bottom_of(
				buffer_pitch,
				rect,
				position)];
}

#endif
