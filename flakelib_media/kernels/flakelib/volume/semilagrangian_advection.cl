#include <flakelib/volume/at.cl>
#include <flakelib/volume/clamp_int4_to_global_rect.cl>
#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/global_size.cl>
#include <flakelib/volume/interpolate_right_neighborhood.cl>
#include <flakelib/volume/load_right_neighborhood.cl>
#include <flakelib/volume/right_neighborhood4.cl>
#include <flakelib/volume/right_neighborhood.cl>
#include <flakelib/boundary_is_solid.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

kernel void
FLAKELIB_KERNEL_NAME(apply_float4)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	global float4 const *FLAKELIB_KERNEL_ARGUMENT(input),
	global float4 const *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(output),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(dt))
{
	int4 const position =
		flakelib_volume_current_position();

	size_t const current_index =
		flakelib_volume_at(
			buffer_pitch,
			flakelib_volume_global_size(),
			position);

	if(flakelib_boundary_is_solid(boundary,current_index))
	{
		output[current_index] =
			(float4)(
				0.0f,
				0.0f,
				0.0f,
				0.0f);
		return;
	}

	float4 const
		current_velocity =
			velocity[current_index],
		advected_vector =
			convert_float4(
				position) -
			dt *
			current_velocity;

	// Warning, nonconst, so don't merge with above
	float4
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);

	flakelib_volume_right_neighborhood4 ns;

	FLAKELIB_VOLUME_LOAD_RIGHT_NEIGHBORHOOD(
		input,
		ns,
		buffer_pitch,
		flakelib_volume_clamp_int4_to_global_rect(
			convert_int4(
				floors)),
		flakelib_volume_global_size());

	output[current_index] =
		FLAKELIB_VOLUME_INTERPOLATE_RIGHT_NEIGHBORHOOD(
			ns,
			fractions);
}

kernel void
FLAKELIB_KERNEL_NAME(apply_float)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	global float const *FLAKELIB_KERNEL_ARGUMENT(input),
	global float4 const *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float *FLAKELIB_KERNEL_ARGUMENT(output),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(dt))
{
	int4 const position =
		flakelib_volume_current_position();

	size_t const current_index =
		flakelib_volume_at(
			buffer_pitch,
			flakelib_volume_global_size(),
			position);

	if(flakelib_boundary_is_solid(boundary,current_index))
	{
		output[current_index] =
			0.0f;
		return;
	}

	float4 const
		current_velocity =
			velocity[current_index],
		advected_vector =
			convert_float4(
				position) -
			dt *
			current_velocity;

	// Warning, nonconst, so don't merge with above
	float4
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);

	flakelib_volume_right_neighborhood ns;

	FLAKELIB_VOLUME_LOAD_RIGHT_NEIGHBORHOOD(
		input,
		ns,
		buffer_pitch,
		flakelib_volume_clamp_int4_to_global_rect(
			convert_int4(
				floors)),
		flakelib_volume_global_size());

	output[current_index] =
		FLAKELIB_VOLUME_INTERPOLATE_RIGHT_NEIGHBORHOOD(
			ns,
			fractions);
}
