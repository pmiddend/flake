#include <flakelib/kernel_argument.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/boundary_is_solid.cl>
#include <flakelib/planar/global_size.cl>
#include <flakelib/planar/current_position.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/planar/left_of.cl>
#include <flakelib/planar/right_of.cl>
#include <flakelib/planar/top_of.cl>
#include <flakelib/planar/bottom_of.cl>

kernel void
FLAKELIB_KERNEL_NAME(subtract_pressure_gradient_apply)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(pressure),
	global float2 *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch))
{
	int2 const position =
		flakelib_planar_current_position();

	int const
		current_index =
			flakelib_planar_at(
				buffer_pitch,
				position),
		left_index =
			flakelib_planar_left_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		right_index =
			flakelib_planar_right_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		top_index =
			flakelib_planar_top_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		bottom_index =
			flakelib_planar_bottom_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position);

	// Could be replaced by a mix on the output value below (but that would
	// mean a lot of computations for returning 0 in the end)
	if(flakelib_boundary_is_solid(boundary,current_index))
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
