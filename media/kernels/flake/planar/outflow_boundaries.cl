#include <flake/planar/current_position.cl>
#include <flake/planar/at.cl>
#include <flake/planar/left_of.cl>
#include <flake/planar/right_of.cl>
#include <flake/planar/top_of.cl>
#include <flake/planar/bottom_of.cl>
#include <flake/planar/global_size.cl>
#include <flake/kernel_name.cl>
#include <flake/kernel_argument.cl>

/*
This function iterates over all the left border pixels of the image and assigns
a fixed force vector to them. This vector points to the right and has a given
magnitude.
*/
kernel void
FLAKE_KERNEL_NAME(apply)(
	global float2 *FLAKE_KERNEL_ARGUMENT(input),
	uint const FLAKE_KERNEL_ARGUMENT(buffer_pitch))
{
	size_t const i =
		get_global_id(
			0);

	// Left border (fill with force magnitude)
	input[buffer_pitch * i] =
		input[buffer_pitch * i + 1];

	// Top border (fill with top+1 border)
	input[i] =
		input[buffer_pitch + i];

	// Bottom border (fill with bottom-1 border)
	input[(buffer_pitch-1) * buffer_pitch + i] =
		input[(buffer_pitch-2) * buffer_pitch + i];

	// Right border (fill with right-1 border)
	input[i * buffer_pitch + (buffer_pitch-1)] =
		input[i * buffer_pitch + (buffer_pitch-2)];
}
