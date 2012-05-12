#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/volume/right_neighbors/float4.cl>
#include <flakelib/volume/right_neighbors/load.cl>
#include <flakelib/volume/right_neighbors/interpolate.cl>
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
#include <flakelib/random_float_value.cl>
#include <flakelib/normalize_from_range.cl>
#include <flakelib/scale_normalized_value.cl>

float4 const
generate_randomized_position(
	int4 const bounding_rect,
	float4 const f)
{
	float4 result;

	result.x =
		flakelib_random_float_value((f.x + f.z) / f.y);
	result.z =
		flakelib_random_float_value(f.x / f.z + f.y);

	result.y =
		// 1/5.0f + flakelib_random_float_value(f.x - f.z - f.y) / 2.0f;
		(result.x + (1.0f - result.x) * flakelib_random_float_value(f.x - f.z - f.y))/2.0f;

	return
		result * convert_float4(bounding_rect);
}

float
flake_initial_velocity_magnitude(
	float const normalized_size)
{
	float const
		velocity_minimum = 0.5f,
		velocity_maximum = 2.0f;

	return
		FLAKELIB_SCALE_NORMALIZED_VALUE(
			normalized_size,
			velocity_minimum,
			velocity_maximum);
}

float4
flake_initial_velocity(
	float const normalized_size)
{
	return
		(float4)(
			0.0f,
			-flake_initial_velocity_magnitude(
				normalized_size),
			0.0f,
			0.0f);
}

kernel void
FLAKELIB_KERNEL_NAME(initialize_velocities)(
	global float4 *FLAKELIB_KERNEL_ARGUMENT(velocities),
	float const FLAKELIB_KERNEL_ARGUMENT(minimum_size),
	float const FLAKELIB_KERNEL_ARGUMENT(maximum_size))
{
	velocities[get_global_id(0)] =
		flake_initial_velocity(
			FLAKELIB_NORMALIZE_FROM_RANGE(
				maximum_size,
				minimum_size,
				maximum_size));
}

float4
gravity_force(
	float const gravity_magnitude,
	float const normalized_size)
{
	return
		(float4)(
			0.0f,
			-FLAKELIB_SCALE_NORMALIZED_VALUE(
				1.0f - normalized_size,
				0.5f,
				1.5f) *
			gravity_magnitude,
			0.0f,
			0.0f);
}

float4
drag_force(
	float const normalized_size,
	float4 const fluid_velocity)
{
	float const umax =
		flake_initial_velocity_magnitude(
			normalized_size);

	return
		fluid_velocity * fluid_velocity / (umax*umax);
}

kernel void
FLAKELIB_KERNEL_NAME(move)(
	global float4 *FLAKELIB_KERNEL_ARGUMENT(positions),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(velocities),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(fluid_velocity),
	float const FLAKELIB_KERNEL_ARGUMENT(time_delta),
	float const FLAKELIB_KERNEL_ARGUMENT(collision_increment),
	int const FLAKELIB_KERNEL_ARGUMENT(bounding_volume_width),
	int const FLAKELIB_KERNEL_ARGUMENT(bounding_volume_height),
	int const FLAKELIB_KERNEL_ARGUMENT(bounding_volume_depth),
	global float const *FLAKELIB_KERNEL_ARGUMENT(activity),
	global float *FLAKELIB_KERNEL_ARGUMENT(snow_density),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(gravity_magnitude),
	float const FLAKELIB_KERNEL_ARGUMENT(minimum_size),
	float const FLAKELIB_KERNEL_ARGUMENT(maximum_size))
{
	float4 const current_position =
		positions[get_global_id(0)];

	float const normalized_size =
		FLAKELIB_NORMALIZE_FROM_RANGE(
			maximum_size,
			minimum_size,
			maximum_size);


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

		velocities[get_global_id(0)] =
			flake_initial_velocity(
				normalized_size);
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

			velocities[get_global_id(0)] =
				flake_initial_velocity(
					normalized_size);
			return;
		}

		flakelib_volume_right_neighbors_float4 neighbors;
		FLAKELIB_VOLUME_RIGHT_NEIGHBORS_LOAD(
			fluid_velocity,
			neighbors,
			buffer_pitch,
			lefttopback_position,
			bounding_rect);

		float4
			floors,
			fractions =
				fract(
					current_position,
					&floors),
			current_fluid_velocity =
				FLAKELIB_VOLUME_RIGHT_NEIGHBORS_INTERPOLATE(
					neighbors,
					fractions);

		velocities[get_global_id(0)] +=
			time_delta *
			(
			 /*
				gravity_force(
					gravity_magnitude,
					normalized_size) +*/
				drag_force(
					normalized_size,
					current_fluid_velocity)
			);

		positions[get_global_id(0)] +=
			time_delta *
			velocities[get_global_id(0)];
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
	{
		activity[current_index] = 1.0f;
		return;
	}

#define FLAKE_FLAKES_MOVER_LOAD_DENSITY_OPERATION(source,target)\
	target = snow_density[source]

	FLAKELIB_VOLUME_VON_NEUMANN_APPLY_BINARY_OPERATION(
		FLAKE_FLAKES_MOVER_LOAD_DENSITY_OPERATION,
		neighborhood,
		floats);

	float const is_active_because_of_snow_density = 1.0f;

	if(floats.left > is_active_because_of_snow_density || floats.right > is_active_because_of_snow_density || floats.top > is_active_because_of_snow_density || floats.bottom > is_active_because_of_snow_density || floats.front > is_active_because_of_snow_density || floats.back > is_active_because_of_snow_density)
		activity[current_index] = 1.0f;
}
