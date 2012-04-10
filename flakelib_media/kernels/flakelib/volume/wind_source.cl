#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/volume/at.cl>

kernel void FLAKELIB_KERNEL_NAME(apply)(
	global float4 *FLAKELIB_KERNEL_ARGUMENT(input),
	int const FLAKELIB_KERNEL_ARGUMENT(buffer_width),
	uint const FLAKELIB_KERNEL_ARGUMENT(line_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(wind_strength))
{
	int4 const current_position =
		(int4)(
			0,
			get_global_id(0),
			get_global_id(1),
			0);

	int4 const rect_size =
		(int4)(
			buffer_width,
			buffer_width,
			buffer_width,
			0);

	input[
		flakelib_volume_at(
			line_pitch,
			rect_size,
			current_position)] =
		(float4)(
			wind_strength,
			0.0f,
			0.0f,
			0.0f);
}
