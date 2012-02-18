#include "planar/positions.cl"

kernel void
apply(
	/* 0 */float const force_magnitude,
	/* 1 */global float2 *input,
	/* 2 */int const buffer_width)
{
	size_t const i =
		get_global_id(
			0);

	// Left border (fill with force magnitude)
	input[buffer_width * i] =
		(float2)(
			force_magnitude,
			0.0f);

	// Top border (fill with top+1 border)
	input[i] =
		input[buffer_width + i];

	// Bottom border (fill with bottom-1 border)
	input[(buffer_width-1) * buffer_width + i] =
		input[(buffer_width-2) * buffer_width + i];

	// Right border (fill with right-1 border)
	input[i * buffer_width + (buffer_width-1)] =
		input[i * buffer_width + (buffer_width-2)];
}
