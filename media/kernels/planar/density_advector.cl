#include "planar/positions.cl"

kernel void
advect(
	/* 0 */global float const *input,
	/* 1 */global float *output,
	/* 2 */global float2 const *velocity,
	/* 3 */int const buffer_width,
	/* 4 */float const dt)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	size_t const current_index =
		FLAKE_PLANAR_AT(buffer_width,position);

	float2 const current_velocity =
		velocity[current_index];

	float2 const advected_vector =
		convert_float2(position) -
		dt *
		current_velocity;

	int2 advected_lefttop =
		(int2)(
			(int)floor(advected_vector.x),
			(int)floor(advected_vector.y));

	int2 clamped_advected_vector =
		clamp(
			advected_lefttop,
			(int2)(0,0),
			(int2)(buffer_width-1,buffer_width-1));

	float2
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);


	size_t const index_lefttop =
		FLAKE_PLANAR_AT(buffer_width,clamped_advected_vector);

	float const
		left =
			input[index_lefttop],
		right =
			input[FLAKE_PLANAR_RIGHT_OF(buffer_width,clamped_advected_vector)],
		leftbottom =
			input[FLAKE_PLANAR_BOTTOM_OF(buffer_width,clamped_advected_vector)],
		rightbottom =
			input[FLAKE_PLANAR_RIGHT_BOTTOM_OF(buffer_width,clamped_advected_vector)];

	output[current_index] =
		mix(
			mix(
				left,
				right,
				fractions.x),
			mix(
				leftbottom,
				rightbottom,
				fractions.x),
			fractions.y);
}

kernel void
apply_sources(
	global float const *sources,
	global float *density)
{
	int const position =
		get_global_id(
			0);

	if(sources[position] > 0.5f)
		density[position] = 1.0f;
}
