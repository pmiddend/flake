#include <flakelib/planar/at.cl>
#include <flakelib/planar/clamp_int2_to_global_rect.cl>
#include <flakelib/planar/current_position.cl>
#include <flakelib/planar/global_size.cl>
#include <flakelib/planar/interpolate_right_neighborhood.cl>
#include <flakelib/planar/load_right_neighborhood.cl>
#include <flakelib/planar/right_neighborhood.cl>
#include <flakelib/planar/interpolate_right_neighborhood2.cl>
#include <flakelib/planar/load_right_neighborhood2.cl>
#include <flakelib/planar/right_neighborhood2.cl>
#include <flakelib/boundary_is_solid.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

kernel void
FLAKELIB_KERNEL_NAME(apply_float2)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	global float2 const *FLAKELIB_KERNEL_ARGUMENT(input),
	global float2 const *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float2 *FLAKELIB_KERNEL_ARGUMENT(output),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(dt))
{
	int2 const position =
		flakelib_planar_current_position();

	size_t const current_index =
		flakelib_planar_at(
			buffer_pitch,
			position);

	if(flakelib_boundary_is_solid(boundary,current_index))
	{
		output[current_index] =
			(float2)(
				0.0f);
		return;
	}

	float2 const
		current_velocity =
			velocity[current_index],
		advected_vector =
			convert_float2(
				position) -
			dt *
			current_velocity;

	// Warning, nonconst, so don't merge with above
	float2
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);

	struct flakelib_planar_right_neighborhood2 ns;

	flakelib_planar_load_right_neighborhood2(
		input,
		&ns,
		buffer_pitch,
		flakelib_planar_clamp_int2_to_global_rect(
			convert_int2(
				floors)),
		flakelib_planar_global_size());

	output[current_index] =
		flakelib_planar_interpolate_right_neighborhood2(
			&ns,
			fractions);
}

kernel void
FLAKELIB_KERNEL_NAME(apply_float)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	global float const *FLAKELIB_KERNEL_ARGUMENT(input),
	global float2 const *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float *FLAKELIB_KERNEL_ARGUMENT(output),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(dt))
{
	int2 const position =
		flakelib_planar_current_position();

	size_t const current_index =
		flakelib_planar_at(
			buffer_pitch,
			position);

	if(flakelib_boundary_is_solid(boundary,current_index))
	{
		output[current_index] =
			0.0f;
		return;
	}

	float2 const
		current_velocity =
			velocity[current_index],
		advected_vector =
			convert_float2(
				position) -
			dt *
			current_velocity;

	// Warning, nonconst, so don't merge with above
	float2
		floors,
		fractions =
			fract(
				advected_vector,
				&floors);

	struct flakelib_planar_right_neighborhood ns;

	flakelib_planar_load_right_neighborhood(
		input,
		&ns,
		buffer_pitch,
		flakelib_planar_clamp_int2_to_global_rect(
			convert_int2(
				floors)),
		flakelib_planar_global_size());

	output[current_index] =
		flakelib_planar_interpolate_right_neighborhood(
			&ns,
			fractions);
}
