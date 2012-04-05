#include <flakelib/planar/current_position.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

struct __attribute__((packed)) vertex
{
	float2 start_position;
	float2 end_position;
	float2 arrow_tip_1_start;
	float2 arrow_tip_1_end;
	float2 arrow_tip_2_start;
	float2 arrow_tip_2_end;
};

kernel void
FLAKELIB_KERNEL_NAME(to_arrow_vb)(
	global struct vertex *FLAKELIB_KERNEL_ARGUMENT(vb),
	global float2 const *FLAKELIB_KERNEL_ARGUMENT(buffer),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	float const FLAKELIB_KERNEL_ARGUMENT(grid_scale),
	float const FLAKELIB_KERNEL_ARGUMENT(arrow_scale))
{
	int2 const position =
		flakelib_planar_current_position();

	int const
		current_index =
			flakelib_planar_at(
				buffer_pitch,
				position);

	float2 const position_float =
		convert_float2(
			position);

	float2 const
		this_arrow =
			buffer[current_index],
		start_position =
			grid_scale * (position_float + 0.5f),
		end_position =
			start_position + arrow_scale * (float2)(this_arrow.x,this_arrow.y),
		first_normal =
			fast_normalize(
				(float2)(
					this_arrow.y,-this_arrow.x)),
		second_normal =
			fast_normalize(
				(float2)(
					-this_arrow.y,this_arrow.x));

	vb[current_index].start_position = start_position;
	vb[current_index].end_position = end_position;
	vb[current_index].arrow_tip_1_start = end_position;
	vb[current_index].arrow_tip_2_start = end_position;
	vb[current_index].arrow_tip_1_end =
		start_position +
		arrow_scale * this_arrow * 0.8 +
		fast_length(this_arrow) * 0.1 * first_normal;
	vb[current_index].arrow_tip_2_end =
		start_position +
		arrow_scale * this_arrow * 0.8 +
		fast_length(this_arrow) * 0.1 * second_normal;
}

kernel void
FLAKELIB_KERNEL_NAME(scalar_to_texture)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(input),
	global write_only image2d_t FLAKELIB_KERNEL_ARGUMENT(output),
	float const FLAKELIB_KERNEL_ARGUMENT(scaling))
{
	int2 const position =
		flakelib_planar_current_position();

	float const input_value = input[get_image_width(output) * position.y + position.x];

	float const
		positive_part =
			scaling *
			max(0.0f,input_value),
		negative_part =
			scaling *
			fabs(min(0.0f,input_value));

	write_imagef(
		output,
		position,
		(float4)(
			positive_part,
			negative_part,
			0.0f,
			1.0f));
}
