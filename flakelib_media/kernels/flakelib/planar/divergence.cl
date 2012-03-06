#include <flakelib/planar/current_position.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/planar/left_of.cl>
#include <flakelib/planar/right_of.cl>
#include <flakelib/planar/top_of.cl>
#include <flakelib/planar/bottom_of.cl>
#include <flakelib/planar/global_size.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

/**
\brief Calculates the divergence, assuming the boundary elements have zero velocity
*/
kernel void
FLAKELIB_KERNEL_NAME(apply)(
	global float2 const *FLAKELIB_KERNEL_ARGUMENT(input),
	global float *FLAKELIB_KERNEL_ARGUMENT(output),
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

	// Here, if the boundary is 1, we set the input to zero.
	// Instead of doing that, we could set it to the obstacle's velocity.
	float2
		left =
			(1.0f - boundary[left_index]) * input[left_index],
		right =
			(1.0f - boundary[right_index]) * input[right_index],
		top =
			(1.0f - boundary[top_index]) * input[top_index],
		bottom =
			(1.0f - boundary[bottom_index]) * input[bottom_index];

	output[current_index] =
		((right.x - left.x) + (bottom.y - top.y)) / 2.0f;
}
