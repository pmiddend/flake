#include "float_handling.cl"
#include "positions.cl"

kernel void
advect(
	/* 0 */global flake_real const *input,
	/* 1 */global flake_real *output,
	/* 2 */global flake_real2 const *velocity,
	/* 3 */int const buffer_width,
	/* 4 */flake_real const dt,
	/* 5 */flake_real const grid_scale)
{
	int2 const position =
		(int2)(
			get_global_id(
				0),
			get_global_id(
				1));

	size_t const current_index =
		FLAKE_AT(buffer_width,position);

	flake_real2 const current_velocity =
		velocity[current_index];

	flake_real2 const advected_vector =
		FLAKE_CONVERT_REAL2(position) -
		dt *
		(FLAKE_REAL_LIT(1.0) / grid_scale) *
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

	flake_real2
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);


	size_t const index_lefttop =
		FLAKE_AT(buffer_width,clamped_advected_vector);

	flake_real const
		left =
			input[index_lefttop],
		right =
			input[FLAKE_RIGHT_OF(buffer_width,clamped_advected_vector)],
		leftbottom =
			input[FLAKE_BOTTOM_OF(buffer_width,clamped_advected_vector)],
		rightbottom =
			input[FLAKE_RIGHT_BOTTOM_OF(buffer_width,clamped_advected_vector)];

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
	global flake_real const *sources,
	global flake_real *density)
{
	int const position =
		get_global_id(
			0);

	if(sources[position] > FLAKE_REAL_LIT(0.5))
		density[position] = FLAKE_REAL_LIT(1.0);
}
