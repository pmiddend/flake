#include <flakelib/planar/current_position.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/planar/left_of.cl>
#include <flakelib/planar/right_of.cl>
#include <flakelib/planar/top_of.cl>
#include <flakelib/planar/bottom_of.cl>
#include <flakelib/planar/global_size.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

kernel void
FLAKELIB_KERNEL_NAME(jacobi_apply)(
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	global float const *FLAKELIB_KERNEL_ARGUMENT(rhs),
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	global float const *FLAKELIB_KERNEL_ARGUMENT(input),
	global float *FLAKELIB_KERNEL_ARGUMENT(output))
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
