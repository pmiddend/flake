#include "volume/positions.cl"

bool
is_solid(
	global float const *boundary,
	size_t const index)
{
	return boundary[index] > 0.5f;
}

kernel void
advect(
	/* 0 */ global float const *boundary,
	/* 1 */ global float4 const *velocity,
	/* 2 */ global float const *input,
	/* 3 */ global float *output,
	/* 4 */ int const buffer_width,
	/* 5 */ float const dt)
{
	int3 const position =
		(int3)(
			get_global_id(0),
			get_global_id(1),
			get_global_id(2));

	size_t const current_index =
		FLAKE_VOLUME_AT(buffer_width,position);

	if(is_solid(boundary,current_index))
	{
		output[current_index] =
			0.0f;
		return;
	}

	float4 const current_velocity =
		velocity[current_index];

	float4 const advected_vector =
		(float4)(position.x,position.y,position.z,0.0f) -
		dt *
		current_velocity;

	int3 advected_lefttopback =
		(int3)(
			(int)floor(advected_vector.x),
			(int)floor(advected_vector.y),
			(int)floor(advected_vector.z));

	int3
		clamped_advected_back_vector =
			clamp(
				advected_lefttopback,
				(int3)(0,0,0),
				(int3)(buffer_width-1,buffer_width-1,buffer_width-1)),
		clamped_advected_front_vector =
			(int3)(
				clamped_advected_back_vector.x,
				clamped_advected_back_vector.y,
				min(
					buffer_width-1,
					clamped_advected_back_vector.z+1));

	float4
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);

	float const
		lefttopback =
			input[FLAKE_VOLUME_AT(buffer_width,clamped_advected_back_vector)],
		righttopback =
			input[FLAKE_VOLUME_RIGHT_OF(buffer_width,clamped_advected_back_vector)],
		leftbottomback =
			input[FLAKE_VOLUME_BOTTOM_OF(buffer_width,clamped_advected_back_vector)],
		rightbottomback =
			input[FLAKE_VOLUME_RIGHT_BOTTOM_OF(buffer_width,clamped_advected_back_vector)],
		lefttopfront =
			input[FLAKE_VOLUME_AT(buffer_width,clamped_advected_front_vector)],
		righttopfront =
			input[FLAKE_VOLUME_RIGHT_OF(buffer_width,clamped_advected_front_vector)],
		leftbottomfront =
			input[FLAKE_VOLUME_BOTTOM_OF(buffer_width,clamped_advected_front_vector)],
		rightbottomfront =
			input[FLAKE_VOLUME_RIGHT_BOTTOM_OF(buffer_width,clamped_advected_front_vector)];

	output[current_index] =
		mix(
			mix(
				mix(
					lefttopback,
					righttopback,
					fractions.x),
				mix(
					leftbottomback,
					rightbottomback,
					fractions.x),
				fractions.y),
			mix(
				mix(
					lefttopfront,
					righttopfront,
					fractions.x),
				mix(
					leftbottomfront,
					rightbottomfront,
					fractions.x),
				fractions.y),
			fractions.z);
}

kernel void
apply_sources(
	global float *density,
	float const density_strength,
	int const buffer_width)
{
	int3 const position =
		(int3)(
			get_global_id(
				0),
			get_global_id(
				1),
			get_global_id(
				2));

	float4 const float_position =
		(float4)(
			position.x,
			position.y,
			position.z,
			0.0f);

	float4 const circle_center =
		(float4)(
			0.0f,
			32.0f,
			32.0f,
			0.0f);

	float const circle_radius = 2.0f;

	if(fast_distance(float_position,circle_center) < circle_radius*circle_radius)
		density[FLAKE_VOLUME_AT(buffer_width,position)] = density_strength;
}
