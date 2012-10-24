#ifndef FLAKELIB_VOLUME_VON_NEUMANN_LOAD_NEIGHBOR_INDICES_CL_INCLUDED
#define FLAKELIB_VOLUME_VON_NEUMANN_LOAD_NEIGHBOR_INDICES_CL_INCLUDED

#include <flakelib/volume/von_neumann/size_t_neighbors.cl>
#include <flakelib/volume/at.cl>
#include <flakelib/volume/left_of.cl>
#include <flakelib/volume/right_of.cl>
#include <flakelib/volume/top_of.cl>
#include <flakelib/volume/bottom_of.cl>
#include <flakelib/volume/front_of.cl>
#include <flakelib/volume/back_of.cl>

flakelib_volume_von_neumann_size_t_neighbors
flakelib_volume_von_neumann_load_neighbor_indices(
	int4 const position,
	int4 const rect,
	uint const buffer_pitch)
{
	flakelib_volume_von_neumann_size_t_neighbors result;

	result.left =
		flakelib_volume_left_of(
			buffer_pitch,
			rect,
			position);
	result.right =
		flakelib_volume_right_of(
			buffer_pitch,
			rect,
			position);
	result.top =
		flakelib_volume_top_of(
			buffer_pitch,
			rect,
			position);
	result.bottom =
		flakelib_volume_bottom_of(
			buffer_pitch,
			rect,
			position);
	result.front =
		flakelib_volume_front_of(
			buffer_pitch,
			rect,
			position);
	result.back =
		flakelib_volume_back_of(
			buffer_pitch,
			rect,
			position);

	return result;
}

#endif
