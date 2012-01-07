#include "positions.cl"

struct __attribute__((packed)) vertex
{
	float4 start_position;
	float4 start_color;
	float4 end_position;
	float4 end_color;
};

kernel void
to_arrow_vb(
	global float4 const *cl_buffer,
	global struct vertex *gl_buffer,
	float const arrow_scale,
	float const grid_scale)
{
	int3 const pos =
		(int3)(
			get_global_id(0),
			get_global_id(1),
			get_global_id(2));

	int const pos_index =
		FLAKE_VOLUME_AT(
			get_global_size(0),
			pos);

	float4 const pos_float =
		(float4)(
			convert_float3(
				pos),
			1.0f);

	gl_buffer[pos_index].start_position =
		grid_scale * (pos_float + 0.5f);

	gl_buffer[pos_index].end_position =
		gl_buffer[pos_index].start_position +
		arrow_scale *
		cl_buffer[
			pos_index];

	gl_buffer[pos_index].start_color =
		(float4)(1.0f,0.0f,0.0f,1.0f);

	gl_buffer[pos_index].end_color =
		(float4)(0.0f,1.0f,0.0f,1.0f);
}
