#include <flakelib/planar/current_position.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/planar/left_of.cl>
#include <flakelib/planar/right_of.cl>
#include <flakelib/planar/top_of.cl>
#include <flakelib/planar/bottom_of.cl>
#include <flakelib/planar/global_size.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

/*
This function iterates over all the left border pixels of the image and assigns
a fixed force vector to them. This vector points to the right and has a given
magnitude.
*/
kernel void
FLAKELIB_KERNEL_NAME(apply)(
	global float2 *FLAKELIB_KERNEL_ARGUMENT(input),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch))
{
	size_t const i =
		get_global_id(
			0);

	/*
	// Left border (fill with force magnitude)
	input[buffer_pitch * i] =
		input[buffer_pitch * i + 1];
		*/

	// Top border (fill with top+1 border)
	// Nope, fill with zero
	input[i] =
		input[buffer_pitch + i];
//		(float2)(0.0f,0.0f);

	// Bottom border (fill with bottom-1 border)
	// Nope, fill with zero
	input[(buffer_pitch-1) * buffer_pitch + i] =
		input[(buffer_pitch-2) * buffer_pitch + i];
//		(float2)(0.0f,0.0f);

	// Right border (fill with right-1 border)
	// Nope, fill with zero
	input[i * buffer_pitch + (buffer_pitch-1)] =
		input[i * buffer_pitch + (buffer_pitch-2)];
//		(float2)(0.0f,0.0f);
}
