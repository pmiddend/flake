#include <flake/planar/at.cl>
#include <flake/planar/clamp_int2_to_global_rect.cl>
#include <flake/planar/current_position.cl>
#include <flake/planar/global_size.cl>
#include <flake/planar/interpolate_right_neighborhood.cl>
#include <flake/planar/load_right_neighborhood.cl>
#include <flake/planar/right_neighborhood.cl>
#include <flake/boundary_is_solid.cl>
#include <flake/kernel_name.cl>
#include <flake/kernel_argument.cl>

kernel void
FLAKE_KERNEL_NAME(apply)(
	global float const *FLAKE_KERNEL_ARGUMENT(boundary),
	global float2 const *FLAKE_KERNEL_ARGUMENT(input),
	global float2 *FLAKE_KERNEL_ARGUMENT(output),
	uint const FLAKE_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKE_KERNEL_ARGUMENT(dt))
{
	int2 const position =
		flake_planar_current_position();

	size_t const current_index =
		flake_planar_at(
			buffer_pitch,
			position);

	if(flake_boundary_is_solid(boundary,current_index))
	{
		output[current_index] =
			(float2)(
				0.0f);
		return;
	}

	float2 const
		current_velocity =
			input[current_index],
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

	struct flake_planar_right_neighborhood ns;

	flake_planar_load_right_neighborhood(
		input,
		&ns,
		buffer_pitch,
		flake_planar_clamp_int2_to_global_rect(
			convert_int2(
				floors)),
		flake_planar_global_size());

	output[current_index] =
		flake_planar_interpolate_right_neighborhood(
			&ns,
			fractions);
}
