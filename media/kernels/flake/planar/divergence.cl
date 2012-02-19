#include <flake/planar/current_position.cl>
#include <flake/planar/at.cl>
#include <flake/planar/left_of.cl>
#include <flake/planar/right_of.cl>
#include <flake/planar/top_of.cl>
#include <flake/planar/bottom_of.cl>
#include <flake/planar/global_size.cl>
#include <flake/kernel_name.cl>
#include <flake/kernel_argument.cl>

/**
\brief Calculates the divergence, assuming the boundary elements have zero velocity
*/
kernel void
FLAKE_KERNEL_NAME(divergence)(
	global float2 const *FLAKE_KERNEL_ARGUMENT(input),
	global float *FLAKE_KERNEL_ARGUMENT(output),
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
