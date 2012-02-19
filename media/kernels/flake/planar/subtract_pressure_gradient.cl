#include <flake/kernel_argument.cl>
#include <flake/kernel_name.cl>
#include <flake/boundary_is_solid.cl>
#include <flake/planar/global_size.cl>
#include <flake/planar/current_position.cl>
#include <flake/planar/at.cl>
#include <flake/planar/left_of.cl>
#include <flake/planar/right_of.cl>
#include <flake/planar/top_of.cl>
#include <flake/planar/bottom_of.cl>

kernel void
FLAKE_KERNEL_NAME(gradient_and_subtract)(
	global float const *FLAKE_KERNEL_ARGUMENT(pressure),
	global float2 *FLAKE_KERNEL_ARGUMENT(velocity),
	global float const *FLAKE_KERNEL_ARGUMENT(boundary),
	uint const FLAKE_KERNEL_ARGUMENT(buffer_pitch))
{
	int2 const position =
		flake_planar_current_position();

	int const
		current_index =
			flake_planar_at(
				buffer_pitch,
				position),
		left_index =
			flake_planar_left_of(
				buffer_pitch,
				flake_planar_global_size(),
				position),
		right_index =
			flake_planar_right_of(
				buffer_pitch,
				flake_planar_global_size(),
				position),
		top_index =
			flake_planar_top_of(
				buffer_pitch,
				flake_planar_global_size(),
				position),
		bottom_index =
			flake_planar_bottom_of(
				buffer_pitch,
				flake_planar_global_size(),
				position);

	// Could be replaced by a mix on the output value below (but that would
	// mean a lot of computations for returning 0 in the end)
	if(flake_boundary_is_solid(boundary,current_index))
	{
		velocity[current_index] =
			(float2)(
				0.0f);
		return;
	}

	float const
		center =
			pressure[current_index],
		left =
			mix(
				pressure[left_index],
				center,
				boundary[left_index]),
		right =
			mix(
				pressure[right_index],
				center,
				boundary[right_index]),
		top =
			mix(
				pressure[top_index],
				center,
				boundary[top_index]),
		bottom =
			mix(
				pressure[bottom_index],
				center,
				boundary[bottom_index]);

	float2 const vmask =
		(float2)(
			1.0f -
			min(
				1.0f,
				boundary[left_index] + boundary[right_index]),
			1.0f -
			min(
				1.0f,
				boundary[top_index] + boundary[bottom_index]));

	float2 const pressure_gradient =
		0.5f *
		(float2)(
			right-left,
			bottom-top);

	velocity[current_index] =
		vmask * (velocity[current_index] - pressure_gradient);
}
