#include "volume/positions.cl"

sampler_t const clamped_nonnormalized_nearest =
	CLK_NORMALIZED_COORDS_FALSE |
	CLK_ADDRESS_CLAMP_TO_EDGE |
	CLK_FILTER_NEAREST;

struct __attribute__((packed)) vertex
//struct vertex
{
	float4 position;
	float4 starting_position;
	float point_size;
};

kernel void
advect(
	/* 0 */int const cube_size,
	/* 1 */global float const *boundary,
	/* 2 */int const particle_count,
	/* 3 */global float4 const *velocity,
	/* 4 */global struct vertex *particles,
	/* 5 */float const dt,
	/* 6 */global read_only image2d_t distribution_input,
	/* 7 */global write_only image2d_t distribution_output)
{
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

	if(any(lefttopback_position < (int3)(0)) || any(lefttopback_position >= (int3)(cube_size-1)))
	{
		if(lefttopback_position.y < 0)
		{
			float2 const
				float_image_size =
					(float2)(
							get_image_width(distribution_output),
							get_image_height(distribution_output)),
				float_cube_size =
					(float2)(
						cube_size,
						cube_size),
				float_texture_coordinates =
					(float2)(
						current_position.x,
						current_position.z) / float_cube_size * float_image_size;

			int2 const
				texture_coordinates =
					clamp(
						(int2)(
							(int)float_texture_coordinates.x,
							(int)float_texture_coordinates.y),
						(int2)(
							0,
							0),
						(int2)(
							get_image_width(distribution_output)-1,
							get_image_height(distribution_output)-1));

			write_imagef(
				distribution_output,
				texture_coordinates,
				read_imagef(
					distribution_input,
					clamped_nonnormalized_nearest,
					texture_coordinates) + 0.20f);
		}

		current_particle->position =
			starting_position;
			//particles[max(0,(int)(current_position.x + current_position.y + current_position.z) % particle_count)].starting_position;
		return;
	}

	if(boundary[FLAKE_VOLUME_AT(cube_size,lefttopback_position)] > 0.2f)
	{
		current_particle->position =
			starting_position;
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
