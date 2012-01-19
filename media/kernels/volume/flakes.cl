#include "volume/positions.cl"

struct __attribute__((packed)) vertex
//struct vertex
{
	float4 position;
	float4 starting_position;
	float point_size;
};

kernel void advect(
	global float4 const *velocity,
	global float const *boundary,
	global struct vertex *particles,
	float const dt,
	int const cube_size)
{
	/*global struct vertex *current_particle =
		particles + get_global_id(0);*/
	global struct vertex *current_particle =
		&particles[get_global_id(0)];

	float4
		current_position =
			current_particle->position,
		starting_position =
			current_particle->starting_position;

	int3 const lefttopback_position =
		(int3)(
			(int)floor(current_position.x),
			(int)floor(current_position.y),
			(int)floor(current_position.z));

	if(any(lefttopback_position < (int3)(0)) || any(lefttopback_position >= (int3)(cube_size-1)) || boundary[FLAKE_VOLUME_AT(cube_size,lefttopback_position)] > 0.2f)
	{
		current_particle->position = starting_position;
		return;
	}

	int3 const lefttopfront_position =
			(int3)(
				lefttopback_position.x,
				lefttopback_position.y,
				lefttopback_position.z+1);

	float4
		floors,
		fractions =
			fract(
				current_particle->position,
				&floors);

	float4 const
		lefttopback =
			velocity[FLAKE_VOLUME_AT(cube_size,lefttopback_position)],
		righttopback =
			velocity[FLAKE_VOLUME_RIGHT_OF(cube_size,lefttopback_position)],
		leftbottomback =
			velocity[FLAKE_VOLUME_BOTTOM_OF(cube_size,lefttopback_position)],
		rightbottomback =
			velocity[FLAKE_VOLUME_RIGHT_BOTTOM_OF(cube_size,lefttopback_position)],
		lefttopfront =
			velocity[FLAKE_VOLUME_AT(cube_size,lefttopfront_position)],
		righttopfront =
			velocity[FLAKE_VOLUME_RIGHT_OF(cube_size,lefttopfront_position)],
		leftbottomfront =
			velocity[FLAKE_VOLUME_BOTTOM_OF(cube_size,lefttopfront_position)],
		rightbottomfront =
			velocity[FLAKE_VOLUME_RIGHT_BOTTOM_OF(cube_size,lefttopfront_position)];

	current_particle->position =
		current_position +
		dt * (float4)(0.0,-1.0,0.0,0.0) +
		dt *
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
