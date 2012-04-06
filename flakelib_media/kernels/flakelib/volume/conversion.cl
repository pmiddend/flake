#include <flakelib/volume/at.cl>
#include <flakelib/volume/current_position.cl>
#include <flakelib/volume/global_size.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

struct __attribute__((packed)) vertex
{
	float4 start_position;
	float4 start_color;
	float4 end_position;
	float4 end_color;
};

kernel void
FLAKELIB_KERNEL_NAME(to_arrow_vb)(
	global float4 const *FLAKELIB_KERNEL_ARGUMENT(input),
	global struct vertex *FLAKELIB_KERNEL_ARGUMENT(output),
	uint const FLAKELIB_KERNEL_ARGUMENT(input_line_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(arrow_scale),
	float const FLAKELIB_KERNEL_ARGUMENT(grid_scale),
	float const FLAKELIB_KERNEL_ARGUMENT(origin_x),
	float const FLAKELIB_KERNEL_ARGUMENT(origin_y),
	float const FLAKELIB_KERNEL_ARGUMENT(origin_z))
{
	int4 const position =
		flakelib_volume_current_position();

	int const position_index =
		flakelib_volume_at(
			input_line_pitch,
			flakelib_volume_global_size(),
			position);

	float4 const position_float =
		(float4)(
			convert_float4(
				position).xyz,
			1.0f);

	float4 const origin_vector = (float4)(origin_x,origin_y,origin_z,0.0f);

	output[position_index].start_position =
		origin_vector +
		grid_scale * (position_float + 0.5f);

	output[position_index].start_position.w =
		1.0f;

	output[position_index].end_position =
		origin_vector +
		output[position_index].start_position +
		arrow_scale *
		input[
			position_index];

	output[position_index].end_position.w = 1.0f;

	output[position_index].start_color =
		(float4)(1.0f,0.0f,0.0f,0.1f);

	output[position_index].end_color =
		(float4)(0.0f,1.0f,0.0f,0.1f);
}

kernel void
FLAKELIB_KERNEL_NAME(float_view_to_flat_volume_texture)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(input),
	uint const FLAKELIB_KERNEL_ARGUMENT(input_line_pitch),
	global write_only image2d_t FLAKELIB_KERNEL_ARGUMENT(output),
	float const FLAKELIB_KERNEL_ARGUMENT(scaling_factor),
	float const FLAKELIB_KERNEL_ARGUMENT(constant_addition),
	uint const FLAKELIB_KERNEL_ARGUMENT(slice_edge_size),
	uint const FLAKELIB_KERNEL_ARGUMENT(slices_per_row))
{
	int4 const volume_position =
		flakelib_volume_current_position();

	int2 const planar_position =
		(int)slice_edge_size *
		(int2)(
			volume_position.z % slices_per_row,
			volume_position.z / slices_per_row) +
		volume_position.xy;

	int const volume_position_index =
		flakelib_volume_at(
			input_line_pitch,
			flakelib_volume_global_size(),
			volume_position);

	float const volume_value =
		constant_addition + scaling_factor * input[volume_position_index];

	write_imagef(
		output,
		planar_position,
		(float4)(
			volume_value,
			volume_value,
			volume_value,
			1.0f));
}
