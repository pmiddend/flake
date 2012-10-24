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
FLAKELIB_KERNEL_NAME(wind_source_apply)(
	float const FLAKELIB_KERNEL_ARGUMENT(wind_strength),
	global float2 *FLAKELIB_KERNEL_ARGUMENT(input),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch))
{
	size_t const i =
		get_global_id(
			0);

	// Left border (fill with force magnitude)
	input[buffer_pitch * i] =
		(float2)(
			wind_strength,
			0.0f);
}
