#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/volume/right_neighborhood4.cl>
#include <flakelib/volume/load_right_neighborhood.cl>
#include <flakelib/volume/interpolate_right_neighborhood.cl>
#include <flakelib/volume/at.cl>
#include <flakelib/volume/von_neumann/load_neighbor_indices.cl>
#include <flakelib/volume/von_neumann/float4_neighbors.cl>
#include <flakelib/volume/von_neumann/float_neighbors.cl>
#include <flakelib/volume/von_neumann/apply_binary_operation.cl>
#include <flakelib/volume/von_neumann/sum.cl>
#include <flakelib/boundary_is_solid.cl>
#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/global_size.cl>
#include <flakelib/boundary_is_solid.cl>

float
float_rng(
	float const f)
{
	return (as_float(as_int(f) & 0x007fffff | 0x40000000) - 3.0f + 1.0f)/2.0f;
}

float4 const
generate_randomized_position(
	int4 const bounding_rect,
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
		result * convert_float4(bounding_rect);
}

kernel void
FLAKELIB_KERNEL_NAME(move)(
	global float4 *FLAKELIB_KERNEL_ARGUMENT(positions),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(velocity),
	float const FLAKELIB_KERNEL_ARGUMENT(time_delta),
	float const FLAKELIB_KERNEL_ARGUMENT(collision_increment),
	int const FLAKELIB_KERNEL_ARGUMENT(bounding_volume_width),
	int const FLAKELIB_KERNEL_ARGUMENT(bounding_volume_height),
	int const FLAKELIB_KERNEL_ARGUMENT(bounding_volume_depth),
	global float const *FLAKELIB_KERNEL_ARGUMENT(activity),
	global float *FLAKELIB_KERNEL_ARGUMENT(snow_density),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch))
{
	float4 const current_position =
		positions[get_global_id(0)];

	int4 const bounding_rect =
		(int4)(
			bounding_volume_width,
			bounding_volume_height,
			bounding_volume_depth,
			1);

	int4 lefttopback_position =
		convert_int4(
			current_position);

	lefttopback_position.w = 0;

	// We're leaving the bounding volume
	if(any(lefttopback_position < (int4)(0)) || any(lefttopback_position >= bounding_rect))
	{
		// Special case: We're leaving it at the bottom
		if(lefttopback_position.y < 0)
		{
			int4 const projected_position =
				(int4)(
					lefttopback_position.x,
					0,
					lefttopback_position.z,
					0);

			// We could still be out of bounds
			if(all(projected_position >= (int4)(0)) && all(projected_position < bounding_rect))
			{
				size_t const projected_position_index =
					flakelib_volume_at(
						buffer_pitch,
						bounding_rect,
						(int4)(
							lefttopback_position.x,
							0,
							lefttopback_position.z,
							0));


				// Project coordinate to the ground, update the snow depth
				snow_density[projected_position_index] +=
					collision_increment;
			}
		}

		positions[get_global_id(0)] =
			generate_randomized_position(
				bounding_rect,
				positions[get_global_id(0)]);
	}
	else
	{
		size_t const current_position_index =
			flakelib_volume_at(
				buffer_pitch,
				bounding_rect,
				lefttopback_position);

		// We've hit a boundary
		//if(flakelib_boundary_is_solid(boundary,current_position_index))
		if(activity[current_position_index] > 0.5f)
		{
			snow_density[current_position_index] +=
				collision_increment;

			positions[get_global_id(0)] =
				generate_randomized_position(
					bounding_rect,
					positions[get_global_id(0)]);
			return;
		}

		flakelib_volume_right_neighborhood4 neighbors;
		FLAKELIB_VOLUME_LOAD_RIGHT_NEIGHBORHOOD(
			velocity,
			neighbors,
			buffer_pitch,
			lefttopback_position,
			bounding_rect);

		float4
			floors,
			fractions =
				fract(
					current_position,
					&floors);

		positions[get_global_id(0)] =
			current_position +
			time_delta * (float4)(0.0,-1.0,0.0,0.0) +
			time_delta *
			FLAKELIB_VOLUME_INTERPOLATE_RIGHT_NEIGHBORHOOD(
				neighbors,
				fractions);
	}
}

kernel void
FLAKELIB_KERNEL_NAME(update_activity)(
	global float *FLAKELIB_KERNEL_ARGUMENT(activity),
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	global float const *FLAKELIB_KERNEL_ARGUMENT(snow_density),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(iso_level))
{
	size_t const current_index =
		flakelib_volume_at(
			buffer_pitch,
			flakelib_volume_global_size(),
			flakelib_volume_current_position());

	float const this_activity =
		activity[current_index];

	if(this_activity > 0.5f || flakelib_boundary_is_solid(boundary,current_index))
		return;

	flakelib_volume_von_neumann_size_t_neighbors neighborhood =
		flakelib_volume_von_neumann_load_neighbor_indices(
			flakelib_volume_current_position(),
			flakelib_volume_global_size(),
			buffer_pitch);

	flakelib_volume_von_neumann_float_neighbors floats;

#define FLAKE_FLAKES_MOVER_LOAD_BOUNDARY_OPERATION(source,target)\
	target = boundary[source]

	FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION(
		FLAKE_FLAKES_MOVER_LOAD_BOUNDARY_OPERATION,
		neighborhood,
		floats);

	float const sum =
		FLAKELIB_VOLUME_VON_NEUMANN_SUM(
			floats);

	if(sum > 0.2f)
		activity[current_index] = 1.0f;

#define FLAKE_FLAKES_MOVER_LOAD_DENSITY_OPERATION(source,target)\
	target = snow_density[source]

	FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION(
		FLAKE_FLAKES_MOVER_LOAD_DENSITY_OPERATION,
		neighborhood,
		floats);

	if(floats.left > iso_level || floats.right > iso_level || floats.top > iso_level || floats.bottom > iso_level || floats.front > iso_level || floats.back > iso_level)
		activity[current_index] = 1.0f;
}
