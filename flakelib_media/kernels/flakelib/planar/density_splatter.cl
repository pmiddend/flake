#include "planar/positions.cl"

kernel void
splat(
	global float *output,
	int const buffer_width,
	int const start_x,
	int const start_y)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	int2 const target_position =
		position +
		(int2)(
			start_x,
			start_y);

	output[FLAKE_PLANAR_AT(buffer_width,target_position)] =
		1.0f;
}