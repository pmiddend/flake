#include <flake/planar/current_position.cl>
#include <flake/planar/at.cl>
#include <flake/planar/left_of.cl>
#include <flake/planar/right_of.cl>
#include <flake/planar/top_of.cl>
#include <flake/planar/bottom_of.cl>
#include <flake/planar/global_size.cl>
#include <flake/kernel_name.cl>
#include <flake/kernel_argument.cl>

kernel void
FLAKE_KERNEL_NAME(jacobi)(
	uint const FLAKE_KERNEL_ARGUMENT(buffer_pitch),
	global float const *FLAKE_KERNEL_ARGUMENT(rhs),
	global float const *FLAKE_KERNEL_ARGUMENT(boundary),
	global float const *FLAKE_KERNEL_ARGUMENT(input),
	global float *FLAKE_KERNEL_ARGUMENT(output))
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

	float const
		center =
			input[current_index],
		left =
			mix(
				input[left_index],
				center,
				boundary[left_index]),
		right =
			mix(
				input[right_index],
				center,
				boundary[right_index]),
		top =
			mix(
				input[top_index],
				center,
				boundary[top_index]),
		bottom =
			mix(
				input[bottom_index],
				center,
				boundary[bottom_index]);

	float const
		rhs_value =
			rhs[current_index],
		result =
			(left + right + top + bottom - rhs_value) * 0.25f;

	output[current_index] =
#ifndef WEIGHTED_JACOBI
		result;
#else
		mix(
			center,
			result,
			2.0f/3.0f);
#endif
}
