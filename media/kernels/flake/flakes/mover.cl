#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/volume/right_neighborhood4.cl>
#include <flakelib/volume/load_right_neighborhood.cl>
#include <flakelib/volume/interpolate_right_neighborhood.cl>
#include <flakelib/volume/at.cl>
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
FLAKELIB_KERNEL_NAME(apply)(
	global float4 *FLAKELIB_KERNEL_ARGUMENT(positions),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(velocity),
	float const FLAKELIB_KERNEL_ARGUMENT(time_delta),
	int const FLAKELIB_KERNEL_ARGUMENT(bounding_volume_width),
	int const FLAKELIB_KERNEL_ARGUMENT(bounding_volume_height),
	int const FLAKELIB_KERNEL_ARGUMENT(bounding_volume_depth),
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
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
			// Project coordinate to the ground, update the snow depth
			//
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
		if(flakelib_boundary_is_solid(boundary,current_position_index))
		{
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
