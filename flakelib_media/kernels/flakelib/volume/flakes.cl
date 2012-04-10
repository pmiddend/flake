#if 0
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

float
float_rng(
	float const f)
{
	return (as_float(as_int(f) & 0x007fffff | 0x40000000) - 3.0f + 1.0f)/2.0f;
}

float4 const
new_position_randomized(
	float4 const borders,
	float4 const f)
{
	float4 result;


	result.x = 
		float_rng((f.x + f.z) / f.y);
	result.z = 
		float_rng(f.x / f.z + f.y);

	result.y = 
		// 1/5.0f + float_rng(f.x - f.z - f.y) / 2.0f;
		(result.x + (1.0f - result.x) * float_rng(f.x - f.z - f.y))/2.0f;

	return
		result * borders;
}

kernel void
advect(
	/* 0 */int const cube_size,
	/* 1 */global float const *boundary,
	/* 2 */int const particle_count,
	/* 3 */global float4 *positions,
	/* 4 */global float4 *starting_positions,
	/* 5 */global float const *point_sizes,
	/* 6 */global float4 const *velocity,
	/* 7 */float const dt,
	/* 8 */global read_only image2d_t distribution_input,
	/* 9 */global write_only image2d_t distribution_output)
{
	float4
		current_position =
			positions[get_global_id(0)],
		starting_position =
			starting_positions[get_global_id(0)];

	int3 const lefttopback_position =
		(int3)(
			(int)floor(current_position.x),
			(int)floor(current_position.y),
			(int)floor(current_position.z));

	float4 cube_size_vector = 
		(float4)(
				cube_size,
				cube_size,
				cube_size,
				0.0f);

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

		positions[get_global_id(0)] =
			new_position_randomized(
				cube_size_vector,
				positions[get_global_id(0)]);
			//starting_position;
			//particles[max(0,(int)(current_position.x + current_position.y + current_position.z) % particle_count)].starting_position;
		return;
	}

	if(lefttopback_position.y > 1 && boundary[FLAKE_VOLUME_AT(cube_size,lefttopback_position)] > 0.2f)
	{
		positions[get_global_id(0)] =
			new_position_randomized(
				cube_size_vector,
				positions[get_global_id(0)]);
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
				positions[get_global_id(0)],
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

	positions[get_global_id(0)] =
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
#endif
