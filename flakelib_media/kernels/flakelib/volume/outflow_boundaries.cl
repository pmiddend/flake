#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/volume/at.cl>

kernel void FLAKELIB_KERNEL_NAME(outflow_boundaries_apply)(
	global float4 *FLAKELIB_KERNEL_ARGUMENT(input),
	int const FLAKELIB_KERNEL_ARGUMENT(buffer_width),
	int const FLAKELIB_KERNEL_ARGUMENT(buffer_height),
	int const FLAKELIB_KERNEL_ARGUMENT(buffer_depth),
	uint const FLAKELIB_KERNEL_ARGUMENT(line_pitch))
{
	int2 const current_position =
		(int2)(
			get_global_id(0),
			get_global_id(1));

	int4 const rect_size =
		(int4)(
			buffer_width,
			buffer_height,
			buffer_depth,
			0);

	if(
		current_position.x < buffer_height)
	{
		// Right
		input[
			flakelib_volume_at(
				line_pitch,
				rect_size,
				(int4)(
					buffer_width-1,
					current_position.x,
					current_position.y,
					0))] =
				input[
					flakelib_volume_at(
						line_pitch,
						rect_size,
						(int4)(
							buffer_width-2,
							current_position.x,
							current_position.y,
							0))];
	}

	// Top
	input[
		flakelib_volume_at(
			line_pitch,
			rect_size,
			(int4)(
				current_position.x,
				buffer_height-1,
				current_position.y,
				0))] =
			input[
				flakelib_volume_at(
					line_pitch,
					rect_size,
					(int4)(
						current_position.x,
						buffer_height-2,
						current_position.y,
						0))];

	// Bottom
	input[
		flakelib_volume_at(
			line_pitch,
			rect_size,
			(int4)(
				current_position.x,
				0,
				current_position.y,
				0))] =
			input[
				flakelib_volume_at(
					line_pitch,
					rect_size,
					(int4)(
						current_position.x,
						1,
						current_position.y,
						0))];
}
