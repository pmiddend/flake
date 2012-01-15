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
	/* 0 */ global float4 const *velocity,
	/* 1 */ global float const *input,
	/* 2 */ global float *output,
	/* 3 */ global float const *boundary,
	/* 4 */ int const buffer_width,
	/* 5 */ float const dt,
	/* 6 */ float const grid_scale)
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
		(1.0f / grid_scale) *
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
	global float const *sources,
	global float *density,
	float const density_strength)
{
	int const position =
		get_global_id(
			0);

	int
		x =
			position % 64,
		y =
			position % (64*64) / 64,
		z =
			position / (64*64);
	//if(sources[position] > 0.5f || position == 10 * 64 * 64/* + 32 * 64*/)
	//if(sources[position] > 0.5f || (position >= 32*64*64+32*64 && position < 32*64*64+32*64 + 10))

	if(sources[position] > 0.5f || (x == 0 && y >= 25 && y <= 39 && z >= 25 && z <= 39))
		density[position] = density_strength;
}
